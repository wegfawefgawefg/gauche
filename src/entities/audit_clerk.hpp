#pragma once
#include "../game.hpp"
enum ClerkPhase { ClerkSeek, ClerkStamp, ClerkReturn, ClerkDeposit, ClerkAlarm, ClerkFlee, ClerkRest };
void init_audit_clerk(Entity& actor);
void step_audit_clerk(Game& game,int slot);
void hurt_audit_clerk(Game& game,int slot,Cell source);
void alarm_pay_clerks(Game& game,Cell cage,Cell source);
void interrupt_audit_clerk(Entity& actor);
bool valid_audit_clerk(const Entity& actor);
