#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "computation_graph.h"
#include <unordered_map>

class Optimizer {
public:
    Optimizer(const ComputationGraph& input_graph);
    ComputationGraph optimize();
    
private:
    ComputationGraph graph;
    std::unordered_map<std::string, int> table_sizes; // Mock table sizes for cost estimation

    std::shared_ptr<ComputationNode> optimizeJoins(const std::shared_ptr<ComputationNode>& node);
    int estimateCost(const std::shared_ptr<ComputationNode>& node);
};

#endif