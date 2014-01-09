#include <iostream>
#include <string>
#include <dnn.h>
#include <dnn-utility.h>
#include <cmdparser.h>
#include <rbm.h>
using namespace std;

int main (int argc, char* argv[]) {

  CmdParser cmd(argc, argv);

  cmd.add("training_set_file")
    .add("model_file", false);

  cmd.addGroup("Structure of Neural Network: ")
     .add("--nodes", "specify the width(nodes) of each hidden layer seperated by \"-\":\n"
	"Ex: 1024-1024-1024 for 3 hidden layer, each with 1024 nodes. \n"
	"(Note: This does not include input and output layer)");

  cmd.addGroup("Pre-training options:")
     .add("--rescale", "Rescale each feature to [0, 1]", "false")
     .add("--slope-thres", "threshold of ratio of slope in RBM pre-training", "0.05")
     .add("--batch-size", "number of data per mini-batch", "32")
     .add("--pre", "type of Pretraining. Choose one of the following:\n"
	"0 -- RBM (Restricted Boltzman Machine)", "0");

  cmd.addGroup("Example usage: dnn-init data/train3.dat --nodes=16-8");

  if (!cmd.isOptionLegal())
    cmd.showUsageAndExit();

  string train_fn   = cmd[1];
  string model_fn   = cmd[2];
  string structure  = cmd["--nodes"];
  size_t batchSize  = cmd["--batch-size"];
  size_t preTraining= cmd["--pre"];
  bool rescale      = cmd["--rescale"];
  float slopeThres  = cmd["--slope-thres"];

  if (model_fn.empty())
    model_fn = train_fn.substr(train_fn.find_last_of('/') + 1) + ".model";

  DataSet data(train_fn, rescale);
  data.shuffleFeature();
  data.showSummary();

  std::vector<size_t> dims = getDimensionsForRBM(data, structure);

  ERROR_MEASURE err = CROSS_ENTROPY;
  
  // Set configurations
  Config config;

  // Initialize Deep Neural Network
  DNN dnn(config);
  dnn.init(dims, rbminit(data, dims, slopeThres));

  // Save the model
  dnn.save(model_fn);

  return 0;
}