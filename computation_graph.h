#ifndef COMPUTATION_GRAPH_H
#define COMPUTATION_GRAPH_H

#include <string>
#include <vector>
#include <memory>

enum class NodeType {
    TABLE,
    SELECTION,
    JOIN
};

struct ComputationNode {
    NodeType type;
    std::string table_name; // For TABLE nodes
    std::string condition;  // For SELECTION or JOIN conditions
    std::vector<std::shared_ptr<ComputationNode>> children;

    ComputationNode(NodeType t) : type(t) {}
};

class ComputationGraph {
public:
    std::shared_ptr<ComputationNode> root;

    ComputationGraph() = default;
    void printGraph(const std::shared_ptr<ComputationNode>& node, int depth = 0);
    // Temporary method to generate a sample graph
    static ComputationGraph generateSampleGraph();
};

#endif