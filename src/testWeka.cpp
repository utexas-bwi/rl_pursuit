#include <common/WekaParser.h>
#include <common/DecisionTree.h>
#include <fstream>
#include <string>
#include <iostream>

int main(int , const char *[]) {
  WekaParser parser("data/decisionTree-aamas11-20x20.weka");
  boost::shared_ptr<DecisionTree> dt = parser.makeDecisionTree();
  
  std::ifstream in("/home/sam/research/adhoc/branches/avi/actionSummary20x20.arff");
  assert(in.good());

  Features features;
  std::vector<std::string> featureNames;
  std::vector<bool> numeric;
  Features valueMap;
  valueMap["U"] = Action::UP;
  valueMap["D"] = Action::DOWN;
  valueMap["L"] = Action::LEFT;
  valueMap["R"] = Action::RIGHT;
  valueMap["S"] = Action::NOOP;

  std::string str;
  std::string start = "@attribute ";
  int startInd;
  int endInd;
  // read in the header
  while (true) {
    std::getline(in,str);
    if (str.compare(0,start.size(),start) != 0)
      continue;
    break;
  }

  while (true) {
    if (str.compare(0,start.size(),start) != 0)
      break;

    startInd = str.find(" ",start.size()-1);
    endInd = str.find(" ",startInd+1);
    //std::cout << startInd << " " << endInd << " " << str.substr(startInd,endInd-startInd) << std::endl;
    featureNames.push_back(str.substr(startInd+1,endInd-startInd-1));
    numeric.push_back(str.substr(endInd+1) == "numeric");
    std::getline(in,str);
  }
  while (str != "@data")
    std::getline(in,str);

  float val;
  std::string val2;
  Classification c;
  //int count = 10;
  while (!in.eof()) {
    for (unsigned int i = 0; i < featureNames.size(); i++) {
      if (numeric[i]) {
        in >> val;
        in.ignore(1,',');
      } else {
        //std::cout << ((char)in.get()) << std::endl;
        std::getline(in,val2,',');
        val = valueMap[val2];
        //std::cout << val2 << std::endl;
      }
      features[featureNames[i]] = val;
      //std::cout << featureNames[i] << " " << val << " " << (featureNames[i] == "Pred.act") << std::endl;
    }

    features["Next2Prey?"] = features["Next2Prey"];
    features["Greedy"] = features["Greedy.des"];
    features["Team Aware"] = features["TA.des"];
    features["Greedy Prob"] = features["GP.des"];
    features["Prob Dest"] = features["PD.des"];
    features["DeltaXtoP"] = features["DeltaXToPrey"];
    features["DeltaYtoP"] = features["DeltaYToPrey"];
    features["sum"] = 0; // TODO
  
    dt->classify(features,c);
    //count--;
    //if (count <= 0)
      //break;
  }
  in.close();
  std::cout << dt->root << std::endl;
  
  return 0;
}
