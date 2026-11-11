#ifndef GRAPH_H
#define GRAPH_H

#include <QString>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QColor>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <climits>

struct Edge {
    int from;
    int to;
    double length;
    double congestionCoefficient;
    QColor color;

    Edge(int f, int t, double len, double coef = 0.8, QColor c = Qt::green)
        : from(f), to(t), length(len), congestionCoefficient(coef), color(c) {}

    double getWeight() const {
        return length * congestionCoefficient;
    }
};

struct Vertex {
    int id;
    QString name;
    QPointF position;

    Vertex(int i, const QString& n, const QPointF& pos)
        : id(i), name(n), position(pos) {}
};

class Graph {
private:
    QList<Vertex> vertices;
    QList<Edge> edges;
    QMap<int, QList<int>> adjacencyList;

    QColor determineColor(double coefficient) {
        if (coefficient < 1.2) return QColor(0, 200, 0);
        if (coefficient < 1.9) return QColor(255, 140, 0);
        if (coefficient < 3.3) return QColor(255, 0, 0);
        return QColor(148, 0, 211);
    }

public:
    Graph() {}

    void addVertex(const Vertex& vertex) {
        vertices.append(vertex);
    }

    void addEdge(const Edge& edge) {
        edges.append(edge);
        adjacencyList[edge.from].append(edge.to);
        adjacencyList[edge.to].append(edge.from);
    }

    QList<Vertex> getVertices() const { return vertices; }
    QList<Edge> getEdges() const { return edges; }

    QList<int> getNeighbors(int vertexId) const {
        return adjacencyList.value(vertexId);
    }

    Edge* getEdge(int from, int to) {
        for (auto& edge : edges) {
            if ((edge.from == from && edge.to == to) ||
                (edge.from == to && edge.to == from)) {
                return &edge;
            }
        }
        return nullptr;
    }

    void simulateTraffic() {
        static const double trafficLevels[] = { 0.8, 1.2, 1.9, 3.3 };
        for (auto& edge : edges) {
            double randomCoef = trafficLevels[QRandomGenerator::global()->bounded(4)];
            edge.congestionCoefficient = randomCoef;
            edge.color = determineColor(randomCoef);
        }
    }

    QList<int> findShortestPath(int start, int end) {
        QMap<int, double> distances;
        QMap<int, int> previous;
        QList<int> unvisited;

        for (const auto& vertex : vertices) {
            distances[vertex.id] = INT_MAX;
            unvisited.append(vertex.id);
        }

        distances[start] = 0;

        while (!unvisited.isEmpty()) {
            int current = -1;
            double minDist = INT_MAX;

            for (int vertexId : unvisited) {
                if (distances[vertexId] < minDist) {
                    minDist = distances[vertexId];
                    current = vertexId;
                }
            }

            if (current == -1 || minDist == INT_MAX)
                break;

            unvisited.removeOne(current);
            if (current == end) break;

            for (int neighbor : getNeighbors(current)) {
                Edge* edge = getEdge(current, neighbor);
                if (edge) {
                    double altDist = distances[current] + edge->getWeight();
                    if (altDist < distances[neighbor]) {
                        distances[neighbor] = altDist;
                        previous[neighbor] = current;
                    }
                }
            }
        }

        QList<int> path;
        if (!previous.contains(end) && start != end) return path;

        int current = end;
        while (previous.contains(current)) {
            path.prepend(current);
            current = previous[current];
        }
        if (!path.isEmpty() || start == end) path.prepend(start);
        return path;
    }
};

#endif
