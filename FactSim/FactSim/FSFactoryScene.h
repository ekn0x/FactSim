#ifndef FS_FACTORY_SCENE
#define FS_FACTORY_SCENE

#include <QGraphicsScene>
#include <QList>

namespace FS
{

	class FactoryScene : public QGraphicsScene
	{
		Q_OBJECT
	
	public:
		FactoryScene() = delete;
		FactoryScene(int w, int h);
		~FactoryScene() = default;

	private:
		QList<QGraphicsItem*> mSimObject;
	};
}

#endif // FS_FACTORY_SCENE