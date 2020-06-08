#include "caribouApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
caribouApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy DirichletBC, that is, set DirichletBC default for preset = true
  params.set<bool>("use_legacy_dirichlet_bc") = false;

  return params;
}

caribouApp::caribouApp(InputParameters parameters) : MooseApp(parameters)
{
  caribouApp::registerAll(_factory, _action_factory, _syntax);
}

caribouApp::~caribouApp() {}

void
caribouApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"caribouApp"});
  Registry::registerActionsTo(af, {"caribouApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
caribouApp::registerApps()
{
  registerApp(caribouApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
caribouApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  caribouApp::registerAll(f, af, s);
}
extern "C" void
caribouApp__registerApps()
{
  caribouApp::registerApps();
}
