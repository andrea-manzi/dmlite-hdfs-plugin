/// @file   c/PluginManager-C.cpp
/// @brief  C wrapper for dmlite::PluginManager.
/// @author Alejandro Álvarez Ayllon <aalvarez@cern.ch>
#include <dmlite/c/dmlite.h>
#include <dmlite/cpp/authn.h>
#include <dmlite/cpp/dmlite.h>
#include "Private.h"



unsigned dmlite_api_version(void)
{
  return dmlite::API_VERSION;
}



dmlite_manager* dmlite_manager_new(void)
{
  dmlite_manager* handle;

  handle = new dmlite_manager;
  handle->manager = new dmlite::PluginManager();
  
  return handle;
}



int dmlite_manager_free(dmlite_manager* handle)
{
  if (handle != NULL) {
    delete handle->manager;
    delete handle;
  }
  return 0;
}



int dmlite_manager_load_plugin(dmlite_manager* handle, const char* lib, const char* id)
{
  if (handle == NULL)
    return DM_NULL_POINTER;

  TRY_CATCH(handle, loadPlugin, lib, id);
}



int dmlite_manager_set(dmlite_manager* handle, const char* key, const char* value)
{
  if (handle == NULL)
    return -1;

  TRY_CATCH(handle, configure, key, value);
}



int dmlite_manager_load_configuration(dmlite_manager* handle, const char* file)
{
  if (handle == NULL)
    return DM_NULL_POINTER;

  TRY_CATCH(handle, loadConfiguration, file);
}



int dmlite_manager_errno(dmlite_manager* handle)
{
  if (handle == NULL)
    return DM_NULL_POINTER;

  return handle->errorCode;
}



const char* dmlite_manager_error(dmlite_manager* handle)
{
  if (handle == NULL)
    return "The manager is a NULL pointer";

  return handle->errorString.c_str();
}



dmlite_context* dmlite_context_new(dmlite_manager* handle)
{
  dmlite_context*         ctx;

  if (handle == NULL)
    return NULL;

  ctx = new dmlite_context();
  ctx->errorCode = 0;
  ctx->stack   = 0x00;
  
  try {
    ctx->stack = new dmlite::StackInstance(handle->manager);
  }
  catch (dmlite::DmException& e) {
    handle->errorCode   = e.code();
    handle->errorString = e.what();
    delete ctx;
    return NULL;
  }

  return ctx;
}



int dmlite_context_free(dmlite_context* context)
{
  if (context != NULL) {
    delete context->stack;
    delete context;
  }
  return 0;
}

int dmlite_errno(dmlite_context* context)
{
  if (context == NULL)
    return DM_NULL_POINTER;
  else
    return context->errorCode;
}



const char* dmlite_error(dmlite_context* context)
{
  if (context == NULL)
    return "The context is a NULL pointer";
  return context->errorString.c_str();
}



int dmlite_setcredentials(dmlite_context* context, dmlite_credentials* cred)
{
  TRY(context, setcredentials)
  NOT_NULL(cred);
  dmlite::SecurityCredentials credpp;
  
  if (cred->extra != NULL)
    credpp.copy(cred->extra->extensible);
  
  credpp.mech          = SAFE_STRING(cred->mech);
  credpp.clientName    = SAFE_STRING(cred->client_name);
  credpp.remoteAddress = SAFE_STRING(cred->remote_address);
  credpp.sessionId     = SAFE_STRING(cred->session_id);
  
  for (unsigned i = 0; i < cred->nfqans; ++i)
    credpp.fqans.push_back(cred->fqans[i]);
  
  context->stack->setSecurityCredentials(credpp);
  CATCH(context, setcredentials)
}



int dmlite_set(dmlite_context* context, const char* k, const dmlite_any* v)
{
  TRY(context, set)
  NOT_NULL(k);
  context->stack->set(k, v->value);
  CATCH(context, set)
}
