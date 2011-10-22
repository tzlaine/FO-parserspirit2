// -*- C++ -*-
#ifndef _Parse_h_
#define _Parse_h_

#include "../universe/Tech.h"

#include <boost/filesystem/path.hpp>


class Alignment;
class BuildingType;
class FleetPlan;
class HullType;
class MonsterFleetPlan;
class PartType;
class ShipDesign;
class Special;
class Species;

namespace parse {

    void init();

    void buildings(const boost::filesystem::path& path, std::map<std::string, BuildingType*>& building_types);

    void specials(const boost::filesystem::path& path, std::map<std::string, Special*>& specials_);

    void species(const boost::filesystem::path& path, std::map<std::string, Species*>& species_);

    void techs(const boost::filesystem::path& path, TechManager::TechContainer& techs_, std::map<std::string, TechCategory*>& tech_categories);

    void items(const boost::filesystem::path& path, std::vector<ItemSpec>& items_);

    void ship_parts(const boost::filesystem::path& path, std::map<std::string, PartType*>& parts);

    void ship_hulls(const boost::filesystem::path& path, std::map<std::string, HullType*>& hulls);

    void ship_designs(const boost::filesystem::path& path, std::map<std::string, ShipDesign*>& designs);

    void fleet_plans(const boost::filesystem::path& path, std::vector<FleetPlan*>& fleet_plans_);

    void monster_fleet_plans(const boost::filesystem::path& path, std::vector<MonsterFleetPlan*>& monster_fleet_plans_);

    void alignments(const boost::filesystem::path& path,
                    std::vector<Alignment>& alignments_,
                    std::vector<boost::shared_ptr<const Effect::EffectsGroup> >& effects_groups);

}

#endif
