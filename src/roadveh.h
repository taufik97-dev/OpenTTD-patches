/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file src/roadveh.h Road vehicle states */

#ifndef ROADVEH_H
#define ROADVEH_H

#include "ground_vehicle.hpp"
#include "engine_base.h"
#include "cargotype.h"
#include "track_func.h"
#include "road_type.h"
#include "newgrf_engine.h"
#include "pathfinder/pos.h"

struct RoadVehicle;

/** Road vehicle states */
enum RoadVehicleStates {
	/*
	 * Lower 4 bits are used for vehicle track direction. (Trackdirs)
	 * When in a road stop (bit 4 or bit 5 set) these bits give the
	 * track direction of the entry to the road stop.
	 * As the entry direction will always be a diagonal
	 * direction (X_NE, Y_SE, X_SW or Y_NW) only bits 0 and 3
	 * are needed to hold this direction. Bit 1 is then used to show
	 * that the vehicle is using the second road stop bay.
	 * Bit 2 is then used for drive-through stops to show the vehicle
	 * is stopping at this road stop.
	 */

	/* Numeric values */
	RVSB_IN_DEPOT                = 0xFE,                      ///< The vehicle is in a depot
	RVSB_WORMHOLE                = 0xFF,                      ///< The vehicle is in a tunnel and/or bridge

	/* Bit numbers */
	RVS_USING_SECOND_BAY         =    1,                      ///< Only used while in a road stop
	RVS_ENTERED_STOP             =    2,                      ///< Only set when a vehicle has entered the stop
	RVS_IN_ROAD_STOP             =    4,                      ///< The vehicle is in a road stop
	RVS_IN_DT_ROAD_STOP          =    5,                      ///< The vehicle is in a drive-through road stop

	/* Bit sets of the above specified bits */
	RVSB_IN_ROAD_STOP            = 1 << RVS_IN_ROAD_STOP,     ///< The vehicle is in a road stop
	RVSB_IN_ROAD_STOP_END        = RVSB_IN_ROAD_STOP + TRACKDIR_END,
	RVSB_IN_DT_ROAD_STOP         = 1 << RVS_IN_DT_ROAD_STOP,  ///< The vehicle is in a drive-through road stop
	RVSB_IN_DT_ROAD_STOP_END     = RVSB_IN_DT_ROAD_STOP + TRACKDIR_END,

	RVSB_TRACKDIR_MASK           = 0x0F,                      ///< The mask used to extract track dirs
	RVSB_ROAD_STOP_TRACKDIR_MASK = 0x09,                      ///< Only bits 0 and 3 are used to encode the trackdir for road stops
};

/** State information about the Road Vehicle controller */
static const uint RDE_NEXT_TILE = 0x80; ///< We should enter the next tile
static const uint RDE_TURNED    = 0x40; ///< We just finished turning

struct RoadDriveEntry {
	byte x, y;
};

extern const RoadDriveEntry * const _road_drive_data[2][2 * TRACKDIR_END];

/* Start frames for when a vehicle enters a tile/changes its state.
 * The start frame is different for vehicles that turned around or
 * are leaving the depot as the do not start at the edge of the tile.
 * For trams there are a few different start frames as there are two
 * places where trams can turn. */
static const uint RVC_DEFAULT_START_FRAME                =  0;
static const uint RVC_SHORT_TURN_START_FRAME             = 16;
static const uint RVC_LONG_TURN_START_FRAME              =  0;
static const uint RVC_AFTER_TURN_START_FRAME             =  1;
static const uint RVC_DEPOT_START_FRAME                  =  6;
/* Stop frame for a vehicle in a drive-through stop */
static const uint RVC_DRIVE_THROUGH_STOP_FRAME           = 11;
static const uint RVC_DEPOT_STOP_FRAME                   = 11;

/** The number of ticks a vehicle has for overtaking. */
static const byte RV_OVERTAKE_TIMEOUT = 35;

void RoadVehUpdateCache(RoadVehicle *v, bool same_length = false);
void GetRoadVehSpriteSize(EngineID engine, uint &width, uint &height, int &xoffs, int &yoffs, EngineImageType image_type);

/**
 * Buses, trucks and trams belong to this class.
 */
struct RoadVehicle FINAL : public GroundVehicle<RoadVehicle, VEH_ROAD> {
	byte state;             ///< @see RoadVehicleStates
	byte frame;
	uint16 blocked_ctr;
	byte overtaking;        ///< Set to 1 when overtaking, otherwise 0.
	byte overtaking_ctr;    ///< The length of the current overtake attempt.
	uint16 crashed_ctr;     ///< Animation counter when the vehicle has crashed. @see RoadVehIsCrashed
	byte reverse_ctr;

	RoadType roadtype;
	RoadTypes compatible_roadtypes;

	/** We don't want GCC to zero our struct! It already is zeroed and has an index! */
	RoadVehicle() : GroundVehicle <RoadVehicle, VEH_ROAD> () {}
	/** We want to 'destruct' the right class. */
	virtual ~RoadVehicle() { this->PreDestructor(); }

	friend struct GroundVehicle<RoadVehicle, VEH_ROAD>; // GroundVehicle needs to use the acceleration functions defined at RoadVehicle.

	void MarkDirty();
	void UpdateDeltaXY(Direction direction);
	ExpensesType GetExpenseType(bool income) const { return income ? EXPENSES_ROADVEH_INC : EXPENSES_ROADVEH_RUN; }
	bool IsPrimaryVehicle() const { return this->IsFrontEngine(); }
	SpriteID GetImage(Direction direction, EngineImageType image_type) const;
	int GetDisplaySpeed() const { return this->gcache.last_speed / 2; }
	int GetDisplayMaxSpeed() const { return this->vcache.cached_max_speed / 2; }
	Money GetRunningCost() const;
	int GetDisplayImageWidth(Point *offset = NULL) const;
	bool IsInDepot() const { return this->state == RVSB_IN_DEPOT; }
	bool Tick();
	void OnNewDay();
	uint Crash(bool flooded = false);
	TileIndex GetOrderStationLocation(StationID station);
	bool FindClosestDepot(TileIndex *location, DestinationID *destination, bool *reverse);

	bool IsBus() const;

	int GetCurrentMaxSpeed() const;
	int UpdateSpeed();

protected: // These functions should not be called outside acceleration code.

	/**
	 * Allows to know the power value that this vehicle will use.
	 * @return Power value from the engine in HP, or zero if the vehicle is not powered.
	 */
	inline uint16 GetPower() const
	{
		/* Power is not added for articulated parts */
		if (!this->IsArticulatedPart()) {
			/* Road vehicle power is in units of 10 HP. */
			return 10 * GetVehicleProperty(this, PROP_ROADVEH_POWER, RoadVehInfo(this->engine_type)->power);
		}
		return 0;
	}

	/**
	 * Returns a value if this articulated part is powered.
	 * @return Zero, because road vehicles don't have powered parts.
	 */
	inline uint16 GetPoweredPartPower(const RoadVehicle *head) const
	{
		return 0;
	}

	/**
	 * Allows to know the weight value that this vehicle will use.
	 * @return Weight value from the engine in tonnes.
	 */
	inline uint16 GetWeight() const
	{
		uint16 weight = (CargoSpec::Get(this->cargo_type)->weight * this->cargo.StoredCount()) / 16;

		/* Vehicle weight is not added for articulated parts. */
		if (!this->IsArticulatedPart()) {
			/* Road vehicle weight is in units of 1/4 t. */
			weight += GetVehicleProperty(this, PROP_ROADVEH_WEIGHT, RoadVehInfo(this->engine_type)->weight) / 4;
		}

		return weight;
	}

	/**
	 * Allows to know the tractive effort value that this vehicle will use.
	 * @return Tractive effort value from the engine.
	 */
	inline byte GetTractiveEffort() const
	{
		/* The tractive effort coefficient is in units of 1/256.  */
		return GetVehicleProperty(this, PROP_ROADVEH_TRACTIVE_EFFORT, RoadVehInfo(this->engine_type)->tractive_effort);
	}

	/**
	 * Gets the area used for calculating air drag.
	 * @return Area of the engine in m^2.
	 */
	inline byte GetAirDragArea() const
	{
		return 6;
	}

	/**
	 * Gets the air drag coefficient of this vehicle.
	 * @return Air drag value from the engine.
	 */
	inline byte GetAirDrag() const
	{
		return RoadVehInfo(this->engine_type)->air_drag;
	}

	/**
	 * Checks the current acceleration status of this vehicle.
	 * @return Acceleration status.
	 */
	inline AccelStatus GetAccelerationStatus() const
	{
		return (this->vehstatus & VS_STOPPED) ? AS_BRAKE : AS_ACCEL;
	}

	/**
	 * Calculates the current speed of this vehicle.
	 * @return Current speed in km/h-ish.
	 */
	inline uint16 GetCurrentSpeed() const
	{
		return this->cur_speed / 2;
	}

	/**
	 * Returns the rolling friction coefficient of this vehicle.
	 * @return Rolling friction coefficient in [1e-4].
	 */
	inline uint32 GetRollingFriction() const
	{
		/* Trams have a slightly greater friction coefficient than trains.
		 * The rest of road vehicles have bigger values. */
		uint32 coeff = (this->roadtype == ROADTYPE_TRAM) ? 40 : 75;
		/* The friction coefficient increases with speed in a way that
		 * it doubles at 128 km/h, triples at 256 km/h and so on. */
		return coeff * (128 + this->GetCurrentSpeed()) / 128;
	}

	/**
	 * Allows to know the acceleration type of a vehicle.
	 * @return Zero, road vehicles always use a normal acceleration method.
	 */
	inline int GetAccelerationType() const
	{
		return 0;
	}

	/**
	 * Returns the slope steepness used by this vehicle.
	 * @return Slope steepness used by the vehicle.
	 */
	inline uint32 GetSlopeSteepness() const
	{
		return _settings_game.vehicle.roadveh_slope_steepness;
	}

	/**
	 * Gets the maximum speed allowed by the track for this vehicle.
	 * @return Since roads don't limit road vehicle speed, it returns always zero.
	 */
	inline uint16 GetMaxTrackSpeed() const
	{
		return 0;
	}
};

#define FOR_ALL_ROADVEHICLES(var) FOR_ALL_VEHICLES_OF_TYPE(RoadVehicle, var)

#endif /* ROADVEH_H */
