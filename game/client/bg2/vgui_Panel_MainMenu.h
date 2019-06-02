#include <vgui/VGUI.h>



namespace vgui
{
	class Panel;
}
 
class ISMenu
{
public:
	virtual void		Create( vgui::VPANEL parent ) = 0;
	virtual void		Destroy( void ) = 0;
	virtual void		Hide() = 0;
};

extern ISMenu *SMenu;
