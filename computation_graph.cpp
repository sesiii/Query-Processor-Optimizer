#include "computation_graph.h"
#include <iostream>

void ComputationGraph::printGraph(const std::shared_ptr<ComputationNode>& node, int depth) {
    if (!node) return;
    std::string indent(depth * 2, ' ');
    if (node->type == NodeType::TABLE) {
        std::cout << indent << "TABLE: " << node->table_name << "\n";
    } else if (node->type == NodeType::SELECTION) {
        std::cout << indent << "SELECTION: " << node->condition << "\n";
    } else if (node->type == NodeType::JOIN) {
        std::cout << indent << "JOIN: " << node->condition << "\n";
    }
    for (const auto& child : node->children) {
        printGraph(child, depth + 1);
    }
}

// Temporary sample graph: SELECT * FROM A JOIN B ON A.id = B.id JOIN C ON B.cid = C.cid WHERE A.x > 10
ComputationGraph ComputationGraph::generateSampleGraph() {
    ComputationGraph graph;

    auto tableA = std::make_shared<ComputationNode>(NodeType::TABLE);
    tableA->table_name = "A";

    auto tableB = std::make_shared<ComputationNode>(NodeType::TABLE);
    tableB->table_name = "B";

    auto tableC = std::make_shared<ComputationNode>(NodeType::TABLE);
    tableC->table_name = "C";

    auto joinAB = std::make_shared<ComputationNode>(NodeType::JOIN);
    joinAB->condition = "A.id = B.id";
    joinAB->children = {tableA, tableB};

    auto joinABC = std::make_shared<ComputationNode>(NodeType::JOIN);
    joinABC->condition = "B.cid = C.cid";
    joinABC->children = {joinAB, tableC};

    auto selection = std::make_shared<ComputationNode>(NodeType::SELECTION);
    selection->condition = "A.x > 10";
    selection->children = {joinABC};

    graph.root = selection;
    return graph;
}