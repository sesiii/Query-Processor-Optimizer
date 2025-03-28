#include "computation_graph.h"
#include "optimizer.h"
#include <iostream>

int main() {
    // Generate a sample computation graph
    ComputationGraph input_graph = ComputationGraph::generateSampleGraph();
    std::cout << "Original Computation Graph:\n";
    input_graph.printGraph(input_graph.root);

    // Optimize the graph
    Optimizer optimizer(input_graph);
    ComputationGraph optimized_graph = optimizer.optimize();
    std::cout << "\nOptimized Computation Graph:\n";
    optimized_graph.printGraph(optimized_graph.root);

    return 0;
}