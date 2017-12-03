/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file airport.h Various declarations for airports */

#ifndef AIRPORT_H
#define AIRPORT_H

#include "map/coord.h"
#include "direction_type.h"

/** Some airport-related constants */
static const uint MAX_TERMINALS =   8;                       ///< maximum number of terminals per airport
static const uint MAX_HELIPADS  =   3;                       ///< maximum number of helipads per airport

static const uint NUM_AIRPORTTILES_PER_GRF = 255;            ///< Number of airport tiles per NewGRF; limited to 255 to allow extending Action3 with an extended byte later on.

static const uint NUM_AIRPORTTILES       = 256;              ///< Total number of airport tiles.
static const uint NEW_AIRPORTTILE_OFFSET = 74;               ///< offset of first newgrf airport tile
static const uint INVALID_AIRPORTTILE    = NUM_AIRPORTTILES; ///< id for an invalid airport tile

/** Airport types */
enum AirportTypes {
	AT_SMALL           =   0, ///< Small airport.
	AT_LARGE           =   1, ///< Large airport.
	AT_HELIPORT        =   2, ///< Heli port.
	AT_METROPOLITAN    =   3, ///< Metropolitan airport.
	AT_INTERNATIONAL   =   4, ///< International airport.
	AT_COMMUTER        =   5, ///< Commuter airport.
	AT_HELIDEPOT       =   6, ///< Heli depot.
	AT_INTERCON        =   7, ///< Intercontinental airport.
	AT_HELISTATION     =   8, ///< Heli station airport.
	AT_OILRIG          =   9, ///< Oilrig airport.
	NEW_AIRPORT_OFFSET =  10, ///< Number of the first newgrf airport.
	NUM_AIRPORTS_PER_GRF = 128, ///< Maximal number of airports per NewGRF.
	NUM_AIRPORTS       = 128, ///< Maximal number of airports in total.
	AT_INVALID         = 254, ///< Invalid airport.
	AT_DUMMY           = 255, ///< Dummy airport.
};

/** Flags for airport movement data. */
enum AirportMovingDataFlags {
	AMED_NOSPDCLAMP = 1 <<  4, ///< No speed restrictions.
	AMED_TAKEOFF    = 1 <<  5, ///< Takeoff movement.
	AMED_SLOWTURN   = 1 <<  6, ///< Turn slowly (mostly used in the air).
	AMED_LAND       = 1 <<  7, ///< Landing onto landing strip.
	AMED_EXACTPOS   = 1 <<  8, ///< Go exactly to the destination coordinates.
	AMED_BRAKE      = 1 <<  9, ///< Taxiing at the airport.
	AMED_HELI_RAISE = 1 << 10, ///< Helicopter take-off.
	AMED_HELI_LOWER = 1 << 11, ///< Helicopter landing.
	AMED_HOLD       = 1 << 12, ///< Holding pattern movement (above the airport).
};

/** Movement States on Airports (headings target) */
enum AirportMovementStates {
	TO_ALL         =  0, ///< Go in this direction for every target.
	HANGAR         =  1, ///< Heading for hangar.
	TERM1          =  2, ///< Heading for terminal 1.
	TERM2          =  3, ///< Heading for terminal 2.
	TERM3          =  4, ///< Heading for terminal 3.
	TERM4          =  5, ///< Heading for terminal 4.
	TERM5          =  6, ///< Heading for terminal 5.
	TERM6          =  7, ///< Heading for terminal 6.
	HELIPAD1       =  8, ///< Heading for helipad 1.
	HELIPAD2       =  9, ///< Heading for helipad 2.
	TAKEOFF        = 10, ///< Airplane wants to leave the airport.
	STARTTAKEOFF   = 11, ///< Airplane has arrived at a runway for take-off.
	ENDTAKEOFF     = 12, ///< Airplane has reached end-point of the take-off runway.
	HELITAKEOFF    = 13, ///< Helicopter wants to leave the airport.
	FLYING         = 14, ///< %Vehicle is flying in the air.
	LANDING        = 15, ///< Airplane wants to land.
	ENDLANDING     = 16, ///< Airplane wants to finish landing.
	HELILANDING    = 17, ///< Helicopter wants to land.
	HELIENDLANDING = 18, ///< Helicopter wants to finish landing.
	TERM7          = 19, ///< Heading for terminal 7.
	TERM8          = 20, ///< Heading for terminal 8.
	HELIPAD3       = 21, ///< Heading for helipad 3.
	MAX_HEADINGS   = 21, ///< Last valid target to head for.
};

/** Movement Blocks on Airports blocks (eg_airport_flags). */
static const uint64
	TERM1_block              = 1ULL <<  0, ///< Block belonging to terminal 1.
	TERM2_block              = 1ULL <<  1, ///< Block belonging to terminal 2.
	TERM3_block              = 1ULL <<  2, ///< Block belonging to terminal 3.
	TERM4_block              = 1ULL <<  3, ///< Block belonging to terminal 4.
	TERM5_block              = 1ULL <<  4, ///< Block belonging to terminal 5.
	TERM6_block              = 1ULL <<  5, ///< Block belonging to terminal 6.
	HELIPAD1_block           = 1ULL <<  6, ///< Block belonging to helipad 1.
	HELIPAD2_block           = 1ULL <<  7, ///< Block belonging to helipad 2.
	RUNWAY_IN_OUT_block      = 1ULL <<  8,
	RUNWAY_IN_block          = 1ULL <<  8,
	AIRPORT_BUSY_block       = 1ULL <<  8,
	RUNWAY_OUT_block         = 1ULL <<  9,
	TAXIWAY_BUSY_block       = 1ULL << 10,
	OUT_WAY_block            = 1ULL << 11,
	IN_WAY_block             = 1ULL << 12,
	AIRPORT_ENTRANCE_block   = 1ULL << 13,
	TERM_GROUP1_block        = 1ULL << 14,
	TERM_GROUP2_block        = 1ULL << 15,
	HANGAR2_AREA_block       = 1ULL << 16,
	TERM_GROUP2_ENTER1_block = 1ULL << 17,
	TERM_GROUP2_ENTER2_block = 1ULL << 18,
	TERM_GROUP2_EXIT1_block  = 1ULL << 19,
	TERM_GROUP2_EXIT2_block  = 1ULL << 20,
	PRE_HELIPAD_block        = 1ULL << 21,

	/* blocks for new airports */
	TERM7_block              = 1ULL << 22, ///< Block belonging to terminal 7.
	TERM8_block              = 1ULL << 23, ///< Block belonging to terminal 8.
	HELIPAD3_block           = 1ULL << 24, ///< Block belonging to helipad 3.
	HANGAR1_AREA_block       = 1ULL << 26,
	OUT_WAY2_block           = 1ULL << 27,
	IN_WAY2_block            = 1ULL << 28,
	RUNWAY_IN2_block         = 1ULL << 29,
	RUNWAY_OUT2_block        = 1ULL << 10, ///< @note re-uses #TAXIWAY_BUSY_block
	HELIPAD_GROUP_block      = 1ULL << 13, ///< @note re-uses #AIRPORT_ENTRANCE_block
	OUT_WAY_block2           = 1ULL << 31,
	/* end of new blocks */

	NOTHING_block            = 1ULL << 30,
	AIRPORT_CLOSED_block     = 1ULL << 63; ///< Dummy block for indicating a closed airport.

/** Finite sTate mAchine (FTA) of an airport. */
struct AirportFTA {
public:
	/** Bitmask of airport flags. */
	enum Flags {
		AIRPLANES   = 0x1,                     ///< Can planes land on this airport type?
		HELICOPTERS = 0x2,                     ///< Can helicopters land on this airport type?
		ALL         = AIRPLANES | HELICOPTERS, ///< Mask to check for both planes and helicopters.
		SHORT_STRIP = 0x4,                     ///< This airport has a short landing strip, dangerous for fast aircraft.
	};

	/** A possible transition between two positions in the machine. */
	struct Transition {
		uint64 block;       ///< 64-bit block (st->airport.flags), should be enough for the most complex airports
		byte heading;       ///< heading (current orders), guiding an airplane to its target on an airport
		byte next_position; ///< next position from this heading
		bool last;          ///< last array element flag
	};

	/** A logical position in the machine. */
	struct Position {
		uint64 block;       ///< 64-bit block (st->airport.flags), should be enough for the most complex airports
		byte heading;       ///< heading (current orders), guiding an airplane to its target on an airport
		byte next_position; ///< next position from this position, if unique
		int16 x;            ///< x-coordinate of the destination
		int16 y;            ///< y-coordinate of the destination
		uint16 flags;       ///< destination direction and special movement flags
		const Transition *transitions; ///< possible transitions, if not unique
	};

	/** A hangar in the machine. */
	struct Hangar {
		uint8 x, y;         ///< Tile coordinates of the hangar.
		uint8 dir;          ///< Direction of the exit.
		uint8 id;           ///< Hangar index.
	};

	const Position *data;                 ///< State machine and movement data.
	const Hangar *hangars;                ///< Airport hangars.
	const byte *terminals;                ///< %Array with the number of terminal groups, followed by the first terminal in each group.
	const byte *entry_points;             ///< when an airplane arrives at this airport, enter it at position entry_point, index depends on direction
	const byte nofelements;               ///< Number of positions in the state machine.
	const byte num_hangars;               ///< Number of hangars in the airport.
	const byte num_helipads;              ///< Number of helipads on this airport. When 0 helicopters will go to normal terminals.
	byte delta_z;                         ///< Z adjustment for helicopter pads
	Flags flags;                          ///< Flags for this airport type.

	template <size_t N, size_t M>
	CONSTEXPR AirportFTA (const Position (&data) [N],
			const byte *terminals, const byte num_helipads,
			const byte *entry_points, Flags flags, byte delta_z,
			const Hangar (&hangars) [M]) :
		data (data), hangars (hangars), terminals (terminals),
		entry_points (entry_points), nofelements (N), num_hangars (M),
		num_helipads (num_helipads), delta_z (delta_z), flags (flags)
	{
		assert_tcompile (N > 0);
		assert_tcompile (N < 256);
		assert_tcompile (M > 0);
		assert_tcompile (M < 256);
	}

	template <size_t N>
	CONSTEXPR AirportFTA (const Position (&data) [N],
			const byte *terminals, const byte num_helipads,
			const byte *entry_points, Flags flags, byte delta_z) :
		data (data), hangars (NULL), terminals (terminals),
		entry_points (entry_points), nofelements (N), num_hangars (0),
		num_helipads (num_helipads), delta_z (delta_z), flags (flags)
	{
		assert_tcompile (N > 0);
		assert_tcompile (N < 256);
	}
};

DECLARE_ENUM_AS_BIT_SET(AirportFTA::Flags)


const AirportFTA *GetAirport (const byte airport_type);
byte GetVehiclePosOnBuild(TileIndex hangar_tile);

#endif /* AIRPORT_H */
