/////////////////////////////////////////////////////////////////////////////
//		File:			KeyBindings.h
//
//		Programmmer:	Garrett Miller (GM)
//
//		Date Created:	2/21/2006 12:50:00 AM
//
//		Description:	Default key bindings
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

enum EMappedActions
{
	eAction_MoveUp,
	eAction_MoveDown,
	eAction_MoveLeft,
	eAction_MoveRight,

	eAction_MoveRun,
	
	eAction_EquipWeapon1,
	eAction_EquipWeapon2,
	eAction_EquipWeapon3,
	eAction_EquipWeapon4,
	eAction_EquipWeapon5,
	eAction_EquipWeapon6,
	
	eAction_Suicide,
	
	eAction_ChangeSpawnType,
	
	eAction_DecreaseSpawnTime,
	eAction_IncreaseSpawnTime,
	
	eAction_StartBulletTime,
	eAction_EndBulletTime,
	
	eAction_Toggle_GodMode,
	eAction_Toggle_FPS,
	
	eAction_CreateRandomBarrels,
	
	eAction_CycleAmbientLightUp,
	eAction_CycleAmbientLightDown,

	eAction_Pause,

	eAction_Spawn4Soldiers,

	eAction_Respawn,

	eAction_MemoryDump,

	eAction_Quit,

	eAction_NumActions
};