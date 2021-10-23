#include "devconsole.hpp"
#include "logging.hpp"

CDevConsole::CDevConsole()
{

}

CDevConsole::~CDevConsole()
{

}

void CDevConsole::Update()
{
	if (m_pInputMan->m_bKeyboardState[KBD_F4] && !m_pInputMan->m_bOldKeyboardState[KBD_F4])
		m_bVisible = !m_bVisible;

	// Rendering
	if (!m_bVisible)
		return;
	
	m_pInputMan->m_bInGui = true;

	// TODO: Redirect stdout

	m_pGui->Image(m_pGui->m_pBGTex, CVector(0, -1), m_pGui->m_vScreenDimensions / m_pGui->m_iGuiUnit, CVector(0, 0.5), Colour(0.2,0.2,0.2));

	m_pGui->Label("]", CVector(0,-1));

	const char* inp = m_pGui->TextInput(DEVCONSOLE_TEXTBUFFER, CVector(1,-1));
	if (inp != nullptr)
	{
		conVarHandle.Parse( inp );
		m_pGui->SetTextBuffer(DEVCONSOLE_TEXTBUFFER, "");
	}
}