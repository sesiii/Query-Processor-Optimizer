#include "optimizer.h"
#include <algorithm>

Optimizer::Optimizer(const ComputationGraph& input_graph) : graph(input_graph) {
    // Mock table sizes for demonstration (in rows)
    table_sizes["A"] = 10;
    table_sizes["B"] = 50000;
    table_sizes["C"] = 2;
}

ComputationGraph Optimizer::optimize() {
    ComputationGraph optimized_graph;
    optimized_graph.root = optimizeJoins(graph.root);
    return optimized_graph;
}

std::shared_ptr<ComputationNode> Optimizer::optimizeJoins(const std::shared_ptr<ComputationNode>& node) {
    if (!node) return nullptr;

    // Recursively optimize children
    for (auto& child : node->children) {
        child = optimizeJoins(child);
    }

    if (node->type != NodeType::JOIN) {
        return node; // Only optimize JOIN nodes
    }

    // For simplicity, assume binary joins. Collect all tables involved.
    std::vector<std::shared_ptr<ComputationNode>> tables;
    std::vector<std::string> conditions;

    auto collectTables = [&](const std::shared_ptr<ComputationNode>& n, auto& self) -> void {
        if (!n) return;
        if (n->type == NodeType::TABLE) {
            tables.push_back(n);
        } else if (n->type == NodeType::JOIN) {
            conditions.push_back(n->condition);
            for (const auto& child : n->children) {
                self(child, self);
            }
        }
    };
    collectTables(node, collectTables);

    // Heuristic: Sort tables by size (smallest first) and rebuild join tree
    std::sort(tables.begin(), tables.end(), [&](const auto& a, const auto& b) {
        return table_sizes[a->table_name] < table_sizes[b->table_name];
    });

    // Rebuild join tree: ((T1 JOIN T2) JOIN T3)
    auto current = tables[0];
    for (size_t i = 1; i < tables.size(); ++i) {
        auto new_join = std::make_shared<ComputationNode>(NodeType::JOIN);
        new_join->condition = conditions[i - 1]; // Simplified: assumes conditions match order
        new_join->children = {current, tables[i]};
        current = new_join;
    }

    return current;
}

int Optimizer::estimateCost(const std::shared_ptr<ComputationNode>& node) {
    if (!node) return 0;
    if (node->type == NodeType::TABLE) {
        return table_sizes[node->table_name];
    }
    if (node->type == NodeType::JOIN) {
        int left_cost = estimateCost(node->children[0]);
        int right_cost = estimateCost(node->children[1]);
        return left_cost + right_cost; // Simplified cost model
    }
    return 0; // Placeholder for other types
}