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

	const char* inp = m_pGui->TextInput(DEVCONSOLE_TEXTBUFFER, m_pGui->m_vScreenCentre / m_pGui->m_iGUIUnitSize);
	if (inp != nullptr)
	{
		conVarHandle.Parse( inp );
		m_pGui->SetTextBuffer(DEVCONSOLE_TEXTBUFFER, "");
	}

	// Auto-complete when hitting tab
	// TODO: Have several auto-completes, cycle through them
	if (m_pInputMan->m_bKeyboardState[KBD_TAB] && !m_pInputMan->m_bOldKeyboardState[KBD_TAB])
	{
		// test against all cvars
		for ( ConVar::CConVar* cvar : conVarHandle.ListConVars())
		{
			if (strncmp(cvar->GetName(), m_pGui->GetTextBuffer(DEVCONSOLE_TEXTBUFFER), strlen(m_pGui->GetTextBuffer(DEVCONSOLE_TEXTBUFFER))) == 0)
			{
				m_pGui->SetTextBuffer(DEVCONSOLE_TEXTBUFFER, cvar->GetName());
				break;
			}
		}
	}
}