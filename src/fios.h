/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file fios.h Declarations for savegames operations */

#ifndef FIOS_H
#define FIOS_H

#include "gfx_type.h"
#include "company_base.h"
#include "gamelog.h"
#include "newgrf_config.h"
#include "network/core/tcp_content.h"
#include "saveload/saveload_data.h"
#include "saveload/saveload_error.h"


typedef SmallMap<uint, CompanyProperties *> CompanyPropertiesMap;

/**
 * Container for loading in mode SL_LOAD_CHECK.
 */
struct LoadCheckData {
	bool checkable;     ///< True if the savegame could be checked by SL_LOAD_CHECK. (Old savegames are not checkable.)
	SlErrorData error;  ///< Error message from loading. INVALID_STRING_ID if no error.

	SavegameTypeVersion sl_version;               ///< Savegame type and version

	uint32 map_size_x, map_size_y;
	Date current_date;

	GameSettings settings;

	CompanyPropertiesMap companies;               ///< Company information.

	GRFConfig *grfconfig;                         ///< NewGrf configuration from save.
	GRFListCompatibility grf_compatibility;       ///< Summary state of NewGrfs, whether missing files or only compatible found.

	Gamelog gamelog;                              ///< Gamelog

	LoadCheckData() : grfconfig(NULL), grf_compatibility(GLC_NOT_FOUND), gamelog()
	{
		this->Clear();
	}

	/**
	 * Don't leak memory at program exit
	 */
	~LoadCheckData()
	{
		this->Clear();
	}

	/**
	 * Check whether loading the game resulted in errors.
	 * @return true if errors were encountered.
	 */
	bool HasErrors()
	{
		return this->checkable && this->error.str != INVALID_STRING_ID;
	}

	/**
	 * Check whether the game uses any NewGrfs.
	 * @return true if NewGrfs are used.
	 */
	bool HasNewGrfs()
	{
		return this->checkable && this->error.str == INVALID_STRING_ID && this->grfconfig != NULL;
	}

	void Clear();
};

extern LoadCheckData _load_check_data;


enum FileSlots {
	/**
	 * Slot used for the GRF scanning and such. This slot cannot be reused
	 * as it will otherwise cause issues when pressing "rescan directories".
	 * It can furthermore not be larger than LAST_GRF_SLOT as that complicates
	 * the testing for "too much NewGRFs".
	 */
	CONFIG_SLOT    =  0,
	/** Slot for the sound. */
	SOUND_SLOT     =  1,
	/** First slot usable for (New)GRFs used during the game. */
	FIRST_GRF_SLOT =  2,
	/** Last slot usable for (New)GRFs used during the game. */
	LAST_GRF_SLOT  = 63,
	/** Maximum number of slots. */
	MAX_FILE_SLOTS = 64
};

/** Mode of the file dialogue window. */
enum SaveLoadDialogMode {
	SLD_LOAD_GAME,      ///< Load a game.
	SLD_LOAD_SCENARIO,  ///< Load a scenario.
	SLD_SAVE_GAME,      ///< Save a game.
	SLD_SAVE_SCENARIO,  ///< Save a scenario.
	SLD_LOAD_HEIGHTMAP, ///< Load a heightmap.
	SLD_SAVE_HEIGHTMAP, ///< Save a heightmap.
};

/** Deals with finding savegames */
struct FiosItem {
	FiosType type;
	uint64 mtime;
	char title[64];
	char name[MAX_PATH];
};

enum SortingBits {
	SORT_ASCENDING  = 0,
	SORT_DESCENDING = 1,
	SORT_BY_DATE    = 0,
	SORT_BY_NAME    = 2
};
DECLARE_ENUM_AS_BIT_SET(SortingBits)

/* Variables to display file lists */
extern SmallVector<FiosItem, 32> _fios_items;
extern SaveLoadDialogMode _saveload_mode;
extern SortingBits _savegame_sort_order;

void ShowSaveLoadDialog(SaveLoadDialogMode mode);

void FiosGetSavegameList(SaveLoadDialogMode mode);
void FiosGetScenarioList(SaveLoadDialogMode mode);
void FiosGetHeightmapList(SaveLoadDialogMode mode);

void FiosFreeSavegameList();
const char *FiosBrowseTo(const FiosItem *item);

StringID FiosGetDescText(const char **path, uint64 *total_free);
bool FiosDelete(const char *name);
void FiosMakeHeightmapName(char *buf, const char *name, size_t size);
void FiosMakeSavegameName(char *buf, const char *name, size_t size);

FiosType FiosGetSavegameListCallback (SaveLoadDialogMode mode, const char *file, const char *ext, stringb *title = NULL);

int CDECL CompareFiosItems(const FiosItem *a, const FiosItem *b);

extern const TextColour _fios_colours[];

void BuildFileList(SaveLoadDialogMode mode);

#endif /* FIOS_H */
