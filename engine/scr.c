#include "scr.h"
#include "angelscript.h"
#include "../common/containers/stack_list.h"


asIScriptEngine *scr_engine;
struct script_t *scr_scripts = NULL;

void scr_Init()
{
    scr_engine = asCreateScriptEngine();
}

void scr_Shutdown()
{

}

