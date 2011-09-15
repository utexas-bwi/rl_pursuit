#include <studentAgents/agents/dikla_ramati/Predator.h>
#include <studentAgents/agents/dina_reidel/Predator.h>
#include <studentAgents/agents/harel_safra/Predator.h>
#include <studentAgents/agents/lior_shlomov/Predator.h>
#include <studentAgents/agents/maxim_leizerovich/Predator.h>
#include <studentAgents/agents/natalie_novitski/Predator.h>
#include <studentAgents/agents/oren_shaivt/Predator.h>
#include <studentAgents/agents/vladimir_torgovitsky/Predator.h>
#include <studentAgents/agents/yosef_digilov/Predator.h>
#include <studentAgents/agents/yura_last/Predator.h>


bool PredatorStudentCpp::handlesStudent(const std::string &name) {
  if (name == "dikla_ramati")
    return true;
  if (name == "dina_reidel")
    return true;
  if (name == "harel_safra")
    return true;
  if (name == "lior_shlomov")
    return true;
  if (name == "maxim_leizerovich")
    return true;
  if (name == "natalie_novitski")
    return true;
  if (name == "oren_shaivt")
    return true;
  if (name == "vladimir_torgovitsky")
    return true;
  if (name == "yosef_digilov")
    return true;
  if (name == "yura_last")
    return true;
  return false;
}

boost::shared_ptr<AbstractCppPredator> PredatorStudentCpp::createPredator(const std::string &name, unsigned int predatorInd) {
  int dims[2];
  bool toroidalWorld = true;
  int moves[Action::NUM_MOVES][2];
  
  dims[0] = this->dims.x;
  dims[1] = this->dims.y;
  
  for (int i = 0; i < 5; i++) {
    moves[i][0] = this->moves[i].x;
    moves[i][1] = this->moves[i].y;
  }
  if (name == "dikla_ramati") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator1(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator2(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator3(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator4(dims,toroidalWorld,moves));
    }
  }
  else if (name == "dina_reidel") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
    }
  }
  else if (name == "harel_safra") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator2(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator2(dims,toroidalWorld,moves));
    }
  }
  else if (name == "lior_shlomov") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator1(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator2(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator3(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator4(dims,toroidalWorld,moves));
    }
  }
  else if (name == "maxim_leizerovich") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
    }
  }
  else if (name == "natalie_novitski") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
    }
  }
  else if (name == "oren_shaivt") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
    }
  }
  else if (name == "vladimir_torgovitsky") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorTopLeft(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorTopRight(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorBottomRight(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorBottomLeft(dims,toroidalWorld,moves));
    }
  }
  else if (name == "yosef_digilov") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
    }
  }
  else if (name == "yura_last") {
    if (predatorInd == 0) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 1) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 2) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
    }
    else if (predatorInd == 3) {
      return boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
    }
  }
  std::cerr << "PredatorStudentCpp::createPredator: ERROR unknown student name: " << name << std::endl;
  exit(6);
}
