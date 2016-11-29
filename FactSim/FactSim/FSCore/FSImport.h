#ifndef FS_IMPORT_H
#define FS_IMPORT_H

#include "FSWorkspace.h"

namespace FS
{
	class Import : public Workspace
	{
	public:
		Import() = default;
		Import(int XPos, int YPos);
		~Import() = default;

	private:

	};

};

#endif // FS_IMPORT_H
