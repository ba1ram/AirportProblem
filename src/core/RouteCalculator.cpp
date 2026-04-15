#include "RouteCalculator.h"
#include <queue>
#include <limits>
#include <algorithm>


RouteCalculator::RouteCalculator(const AirportDatabase& db, double maxRange)
    : m_db(db), m_maxRangeKm(maxRange)
{
    buildGraph(); 
}

void RouteCalculator::setMaxRange(double maxRange)
{
    m_maxRangeKm = maxRange;
    buildGraph();
}

std::vector<std::pair<double, Airport>> RouteCalculator::findNearby(const Airport& target, double radiusKm) const {
    std::vector<std::pair<double, Airport>> nearby;

    for (const auto& airport : m_db.getAllAirports()) {
        if (airport.code() == target.code()) continue;

        double distance = target.distanceTo(airport);
        if (distance <= radiusKm) {
            nearby.push_back({distance, airport});
        }
    }

    // Сортируем по расстоянию
    std::sort(nearby.begin(), nearby.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

    return nearby;
}


void RouteCalculator::buildGraph()
{
    const int n = m_db.size();

    m_graph.clear();
    m_graph.resize(n);

    for (int i = 0; i < n; ++i) {
        const Airport& a = m_db.getByIndex(i);

        for (int j = i + 1; j < n; ++j) {
            const Airport& b = m_db.getByIndex(j);

            double d = a.distanceTo(b);

            if (d <= m_maxRangeKm) {
                m_graph[i].push_back({j, d});
                m_graph[j].push_back({i, d}); // если граф неориентированный
            }
        }
    }
}

RouteCalculator::PathResult RouteCalculator::findShortestPath(
    const QString& from,
    const QString& to) const
{
    PathResult result;
    result.reachable = false;
    result.totalDistance = 0.0;

    const int startIdx  = m_db.getIndex(from);
    const int targetIdx = m_db.getIndex(to);

    if (startIdx == -1 || targetIdx == -1)
        return result;

    const int n = m_db.size();

    using Node = std::pair<double, int>;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    std::vector<double> dist(n, std::numeric_limits<double>::infinity());
    std::vector<int> prev(n, -1);
    std::vector<bool> visited(n, false);

    dist[startIdx] = 0.0;
    pq.push({0.0, startIdx});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        if (u == targetIdx) break;

        for (const auto& [v, weight] : m_graph[u]) {
            double newDist = d + weight;

            if (newDist < dist[v]) {
                dist[v] = newDist;
                prev[v] = u;
                pq.push({newDist, v});
            }
        }
    }

    if (dist[targetIdx] == std::numeric_limits<double>::infinity())
        return result;

    result.reachable = true;
    result.totalDistance = dist[targetIdx];

    std::vector<int> path;
    for (int at = targetIdx; at != -1; at = prev[at])
        path.push_back(at);

    std::reverse(path.begin(), path.end());

    for (int idx : path)
        result.airports.push_back(m_db.getByIndex(idx).code());

    return result;
}
