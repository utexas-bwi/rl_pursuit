#include <controller/PredatorStudentCppAbstract.h>
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
#include <studentAgents/agentsNew/AyalWeissman/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/BorisKodel/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/DrorBanin/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/EranTwili/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/ItaiSchwartz/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/LizBida/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/MenachemShapira/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/MoriaTanami/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/RamMaymon/cppPredator/MyPredator.h>
#include <studentAgents/agentsNew/RomanShif/cppPredator/MyPredator.h>


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
  if (name == "AyalWeissman")
    return true;
  if (name == "BorisKodel")
    return true;
  if (name == "DrorBanin")
    return true;
  if (name == "EranTwili")
    return true;
  if (name == "ItaiSchwartz")
    return true;
  if (name == "LizBida")
    return true;
  if (name == "MenachemShapira")
    return true;
  if (name == "MoriaTanami")
    return true;
  if (name == "RamMaymon")
    return true;
  if (name == "RomanShif")
    return true;
  return false;
}

void PredatorStudentCpp::createPredator(const std::string &name, unsigned int predatorInd) {
  int dims[2];
  bool toroidalWorld = true;
  int moves[Action::NUM_MOVES][2];
  
  dims[0] = this->dims.x;
  dims[1] = this->dims.y;
  
  for (int i = 0; i < 5; i++) {
    moves[i][0] = STUDENT_MOVES_OLD[i].x;
    moves[i][1] = STUDENT_MOVES_OLD[i].y;
  }
  if (name == "dikla_ramati") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator1(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator2(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator3(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dikla_ramati::Predator4(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "dina_reidel") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_dina_reidel::Predator(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "harel_safra") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator2(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_harel_safra::Predator2(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "lior_shlomov") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator1(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator2(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator3(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_lior_shlomov::Predator4(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "maxim_leizerovich") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_maxim_leizerovich::Predator(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "natalie_novitski") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_natalie_novitski::Predator(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "oren_shaivt") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_oren_shaivt::Predator(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "vladimir_torgovitsky") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorTopLeft(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorTopRight(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorBottomRight(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_vladimir_torgovitsky::PredatorBottomLeft(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "yosef_digilov") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yosef_digilov::Predator(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "yura_last") {
    if (predatorInd == 0) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 1) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 2) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
      return;
    }
    else if (predatorInd == 3) {
      predator = boost::shared_ptr<AbstractCppPredator>(new STUDENT_PREDATOR_yura_last::Predator(dims,toroidalWorld,moves));
      return;
    }
  }
  else if (name == "AyalWeissman") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_AyalWeissman::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_AyalWeissman::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_AyalWeissman::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_AyalWeissman::MyPredator(dims));
      return;
    }
  }
  else if (name == "BorisKodel") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_BorisKodel::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_BorisKodel::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_BorisKodel::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_BorisKodel::MyPredator(dims));
      return;
    }
  }
  else if (name == "DrorBanin") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_DrorBanin::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_DrorBanin::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_DrorBanin::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_DrorBanin::MyPredator(dims));
      return;
    }
  }
  else if (name == "EranTwili") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_EranTwili::AbovePredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_EranTwili::BelowPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_EranTwili::RightPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_EranTwili::LeftPredator(dims));
      return;
    }
  }
  else if (name == "ItaiSchwartz") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_ItaiSchwartz::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_ItaiSchwartz::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_ItaiSchwartz::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_ItaiSchwartz::MyPredator(dims));
      return;
    }
  }
  else if (name == "LizBida") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_LizBida::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_LizBida::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_LizBida::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_LizBida::MyPredator(dims));
      return;
    }
  }
  else if (name == "MenachemShapira") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MenachemShapira::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MenachemShapira::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MenachemShapira::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MenachemShapira::MyPredator(dims));
      return;
    }
  }
  else if (name == "MoriaTanami") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MoriaTanami::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MoriaTanami::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MoriaTanami::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_MoriaTanami::MyPredator(dims));
      return;
    }
  }
  else if (name == "RamMaymon") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RamMaymon::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RamMaymon::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RamMaymon::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RamMaymon::MyPredator(dims));
      return;
    }
  }
  else if (name == "RomanShif") {
    if (predatorInd == 0) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RomanShif::MyPredator(dims));
      return;
    }
    else if (predatorInd == 1) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RomanShif::MyPredator(dims));
      return;
    }
    else if (predatorInd == 2) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RomanShif::MyPredator(dims));
      return;
    }
    else if (predatorInd == 3) {
      predatorNew = boost::shared_ptr<PredatorStudentCppAbstract>(new STUDENT_PREDATOR_RomanShif::MyPredator(dims));
      return;
    }
  }
  std::cerr << "PredatorStudentCpp::createPredator: ERROR unknown student name: " << name << std::endl;
  exit(6);
}
