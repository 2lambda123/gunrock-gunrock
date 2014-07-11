#include <gunrock/gunrock.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  // define data types
  struct GunrockDataType data_type;
  data_type.VTXID_TYPE = VTXID_INT;
  data_type.SIZET_TYPE = SIZET_UINT;
  data_type.VALUE_TYPE = VALUE_INT;

  // define graph
  size_t num_nodes = 7;
  size_t num_edges = 15;
  size_t top_nodes = 3;

  unsigned int row_offsets[8] = {0,3,6,9,11,14,15,15};
  int col_indices[15] = {1,2,3,0,2,4,3,4,5,5,6,2,5,6,6};

  unsigned int col_offsets[8] = {0,1,2,5,7,9,12,15};
  int row_indices[15] = {1,0,0,1,4,0,2,1,2,2,3,4,3,4,5};

  // build graph as input
  struct GunrockGraph *graph_input =
    (struct GunrockGraph*)malloc(sizeof(struct GunrockGraph));
  graph_input->num_nodes = num_nodes;
  graph_input->num_edges = num_edges;
  graph_input->row_offsets = (void*)&row_offsets[0];
  graph_input->col_indices = (void*)&col_indices[0];
  graph_input->col_offsets = (void*)&col_offsets[0];
  graph_input->row_indices = (void*)&row_indices[0];

  // malloc output result arrays
  struct GunrockGraph *graph_output =
    (struct GunrockGraph*)malloc(sizeof(struct GunrockGraph));
  int *node_ids          = (int*)malloc(sizeof(int) * top_nodes);
  int *centrality_values = (int*)malloc(sizeof(int) * top_nodes);

  // run topk calculations
  topk_dispatch(
    (struct GunrockGraph*)graph_output,
    node_ids,
    centrality_values,
    (const struct GunrockGraph*)graph_input,
    top_nodes,
    data_type);

  // print results for check correctness
  int i;
  for (i = 0; i < top_nodes; ++i)
  {
    printf("Node ID [%d] : CV [%d] \n", node_ids[i], centrality_values[i]);
  }
  printf("\n");

  if (centrality_values) free(centrality_values);
  if (node_ids)          free(node_ids);
  if (graph_input)       free(graph_input);
  if (graph_output)      free(graph_output);
  return 0;
}