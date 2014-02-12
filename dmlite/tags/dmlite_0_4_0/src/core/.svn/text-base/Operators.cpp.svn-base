/// @file   core/Operators.cpp
/// @brief  Implementation of the operators defined for the simple data structs.
/// @author Alejandro Álvarez Ayllón <aalvarez@cern.ch>
#include <dmlite/cpp/authn.h>

#include "dmlite/cpp/inode.h"
#include "dmlite/cpp/pooldriver.h"
#include "dmlite/cpp/poolmanager.h"

using namespace dmlite;

// Authn

bool SecurityCredentials::operator == (const SecurityCredentials& sc) const
{
  return  this->clientName    == sc.clientName &&
          this->fqans         == sc.fqans &&
          this->mech          == sc.mech &&
          this->remoteAddress == sc.remoteAddress &&
          this->sessionId     == sc.sessionId;
}



bool SecurityCredentials::operator != (const SecurityCredentials& sc) const
{
  return !(*this == sc);
}



bool SecurityCredentials::operator <  (const SecurityCredentials& sc) const
{
  if (this->clientName < sc.clientName)
    return true;
  else if (this->clientName > sc.clientName)
    return false;
  else if (this->fqans < sc.fqans)
    return true;
  else if (this->fqans > sc.fqans)
    return false;
  else
    return this->sessionId < sc.sessionId;
}



bool SecurityCredentials::operator >  (const SecurityCredentials& sc) const
{
  return (*this < sc);
}



bool UserInfo::operator == (const UserInfo& u) const
{
  return this->name == u.name;
}



bool UserInfo::operator != (const UserInfo& u) const
{
  return this->name != u.name;
}



bool UserInfo::operator <  (const UserInfo& u) const
{
  return this->name < u.name;
}


bool UserInfo::operator >  (const UserInfo& u) const
{
  return this->name > u.name;
}


bool GroupInfo::operator == (const GroupInfo& g) const
{
  return this->name == g.name;
}



bool GroupInfo::operator != (const GroupInfo& g) const
{
  return this->name != g.name;
}



bool GroupInfo::operator <  (const GroupInfo& g) const
{
  return this->name < g.name;
}



bool GroupInfo::operator >  (const GroupInfo& g) const
{
  return this->name > g.name;
}



bool SecurityContext::operator == (const SecurityContext& s) const
{
  return this->user == s.user && this->groups == s.groups;
}



bool SecurityContext::operator != (const SecurityContext& s) const
{
  return !(*this == s);
}



bool SecurityContext::operator <  (const SecurityContext& s) const
{
  return (this->user < s.user) ||
         (this->user == s.user && this->groups < s.groups);
}



bool SecurityContext::operator >  (const SecurityContext& s) const
{
  return *this < s;
}

// Inode

bool ExtendedStat::operator == (const ExtendedStat& xs) const
{
  return this->stat.st_ino == xs.stat.st_ino;
}



bool ExtendedStat::operator != (const ExtendedStat& xs) const
{
  return !(*this == xs);
}



bool ExtendedStat::operator <  (const ExtendedStat& xs) const
{
  return this->stat.st_ino < xs.stat.st_ino;
}



bool ExtendedStat::operator >  (const ExtendedStat& xs) const
{
  return *this < xs;
}


bool SymLink::operator == (const SymLink& l) const
{
  return this->inode == l.inode;
}



bool SymLink::operator != (const SymLink& l) const
{
  return !(*this == l);
}



bool SymLink::operator <  (const SymLink& l) const
{
  return this->inode < l.inode;
}



bool SymLink::operator >  (const SymLink& l) const
{
  return *this < l;
}



bool Replica::operator == (const Replica& r) const
{
  return this->replicaid == r.replicaid;
}



bool Replica::operator != (const Replica& r) const
{
  return !(*this == r);
}



bool Replica::operator <  (const Replica& r) const
{
  return this->replicaid < r.replicaid;
}


bool Replica::operator >  (const Replica& r) const
{
  return *this < r;
}

// Pooldriver

bool Chunk::operator == (const Chunk& c) const
{
  return !(*this < c) && !(*this > c);
}



bool Chunk::operator != (const Chunk& c) const
{
  return !(*this == c);
}



bool Chunk::operator <  (const Chunk& c) const
{
  if (this->host < c.host)
    return true;
  else if (this->host > c.host)
    return false;
  else if (this->path < c.path)
    return true;
  else if (this->path > c.path)
    return false;
  else if (this->offset < c.offset)
    return true;
  else if (this->offset > c.offset)
    return false;
  else
    return this->size < c.size;
}



bool Chunk::operator >  (const Chunk& c) const
{
  return *this < c;
}

// Pool Manager

bool Pool::operator == (const Pool& p) const
{
  return this->type == p.type && this->name == p.name;
}



bool Pool::operator != (const Pool& p) const
{
  return !(*this == p);
}



bool Pool::operator <  (const Pool& p) const
{
  if (this->type < p.type)
    return true;
  else if (this->type > p.type)
    return false;
  else
    return this->name == p.name;
}



bool Pool::operator >  (const Pool& p) const
{
  return *this < p;
}
