

#ifndef _ModeCreator_h_
#define _ModeCreator_h_

class Mode;
class Room;

namespace ModeCreator
{
	Mode* CreateMode( Room *pCreator, ModeType eMode );
}

#endif
