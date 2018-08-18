#include "lss/game/content/enemies.hpp"
#include "lss/game/content/items.hpp"
#include "lss/game/itemSpec.hpp"
#include "lss/game/terrain.hpp"
#include "lss/state.hpp"

const std::map<TerrainSpec, std::string> terrainSigns = {
    {TerrainType::TORCH_STAND, "*"},
    {TerrainType::FIREBALL, "*"},
    {TerrainType::FROSTBALL, "*"},
    {TerrainType::ACIDBALL, "*"},
    {TerrainType::MAGIC_LIGHT, "*"},
    {TerrainType::MAGIC_LIGHT_LONG, "*"},
    {TerrainType::MAGIC_LIGHT_FOREVER, "*"},
    {TerrainType::ACID_LIGHT_FOREVER, "*"},
    {TerrainType::STATUE, "&amp;"},
    {TerrainType::ALTAR, "_"},
    // {TerrainType::BUSH, "⇞"},
    {TerrainType::BUSH, "❞"},
};

const std::map<TerrainSpec, std::string> terrainColors = {
    {TerrainType::TORCH_STAND, "#ffa500"},
    {TerrainType::FIREBALL, "#ee1111"},
    {TerrainType::ACIDBALL, "#11cc11"},
    {TerrainType::FROSTBALL, COLORS::LIGHT_BLUE},
    {TerrainType::MAGIC_LIGHT, COLORS::LIGHT_BLUE},
    {TerrainType::MAGIC_LIGHT_LONG, COLORS::LIGHT_BLUE},
    {TerrainType::MAGIC_LIGHT_FOREVER, COLORS::LIGHT_BLUE},
    {TerrainType::ACID_LIGHT_FOREVER, "#11cc11"},
    {TerrainType::STATUE, "#f0f0ff"},
    {TerrainType::ALTAR, COLORS::GRAY},
    {TerrainType::BUSH, "#228b22"},
};

const std::map<ItemSpec, std::string> itemSigns = {
    {ItemType::GRASS, "\""},        {ItemType::SCROLL, "?"},
    {ItemType::POTION, "!"},        {ItemType::ROCK, "*"},
    {ItemType::BONES, "%"},

    {ItemType::ENEMY, "X"},

    {ItemType::PICK_AXE, "("},      {ItemType::TWO_HAND, "("},
    {ItemType::ONE_HAND, "("},      {ItemType::LIGHT_WEAPON, "("},

    {ItemType::RING, "="},          {ItemType::AMULET, "'"},

    {ItemType::TORCH, "]"},         {ItemType::LANTERN, "]"},
    {ItemType::GOLD_COINS, "$"},    {ItemType::PLATE, "["},
    {ItemType::CUIRASS, "["},

    {ItemType::HELMET, "["},        {ItemType::SHIELD, "["},
    {ItemType::GREAVES, "["},       {ItemType::BOOTS, "["},
    {ItemType::CLOAK, "["},

    {ItemType::LEFT_PAULDRON, "["}, {ItemType::RIGHT_PAULDRON, "["},
    {ItemType::LEFT_GAUNTLET, "["}, {ItemType::RIGHT_GAUNTLET, "["},

    {ItemType::CORPSE, "%"},        {ItemType::THROWING_KNIVES, "↑"},
};

const std::map<ItemSpec, std::string> itemColors = {
    {ItemType::BONES, "#aaa"},

    {ItemType::CORPSE, "#ee1111"},
    {ItemType::ROCK, COLORS::GRAY},
    {ItemType::PICK_AXE, COLORS::WHITE},
    {ItemType::ONE_HAND, "#F7CA88"},
    {ItemType::RING, "#ffd700"},
    {ItemType::AMULET, "#ffd700"},
    {ItemType::GOLD_COINS, "#ffd700"},
    {ItemType::LIGHT_WEAPON, COLORS::GRAY},
    {ItemType::TORCH, "#ffa500"},
    {ItemType::LANTERN, "#a5a500"},
    {ItemType::GRASS, "#006400"},

    {ItemType::TWO_HAND, COLORS::WHITE},
    {ItemType::HELMET, COLORS::WHITE},
    {ItemType::SHIELD, COLORS::WHITE},
    {ItemType::GREAVES, COLORS::WHITE},
    {ItemType::BOOTS, COLORS::WHITE},
    {ItemType::PLATE, COLORS::WHITE},
    {ItemType::CUIRASS, COLORS::WHITE},
    {ItemType::CLOAK, COLORS::WHITE},
    {ItemType::POTION, COLORS::LIGHT_BLUE},
    {ItemType::SCROLL, COLORS::WHITE},
    {ItemType::THROWING_KNIVES, COLORS::WHITE},

    {ItemType::LEFT_PAULDRON, COLORS::WHITE},
    {ItemType::RIGHT_PAULDRON, COLORS::WHITE},
    {ItemType::LEFT_GAUNTLET, COLORS::WHITE},
    {ItemType::RIGHT_GAUNTLET, COLORS::WHITE},
};

const std::map<EnemySpec, std::string> enemySigns = {
    {EnemyType::RAT, "r"},
    {EnemyType::BAT, "b"},
    {EnemyType::SNAKE, "s"},
    {EnemyType::VIPER, "s"},
    {EnemyType::BAT_LARGE, "B"},
    {EnemyType::GOBLIN, "g"},
    {EnemyType::ORK_BLACK, "o"},
    {EnemyType::ORK, "o"},
    {EnemyType::ORK_BERSERK, "o"},
    {EnemyType::PIXI, "p"},
    {EnemyType::OGRE, "O"},
    {EnemyType::I_OGRE, "O"},
    {EnemyType::GOBLIN_LIEUTENANT, "g"},
    {EnemyType::GOBLIN_ROGUE, "g"},
    {EnemyType::ANT, "a"},
    {EnemyType::ANT_QUEEN, "a"},
    {EnemyType::SKELETON, "z"},
    {EnemyType::ZOMBIE, "z"},
    {EnemyType::WRAITH, "W"},

    {EnemyType::FIRE_ELEMENTAL, "E"},
    {EnemyType::FROST_ELEMENTAL, "E"},
    {EnemyType::ACID_ELEMENTAL, "E"},

    {EnemyType::STONE_GOLEM, "Y"},
    {EnemyType::STEEL_GOLEM, "Y"},

    {EnemyType::FIRE_DRAGON, "D"},
    {EnemyType::FROST_DRAGON, "D"},
    {EnemyType::ACID_DRAGON, "D"},
};

const std::map<EnemySpec, std::string> enemyColors = {
    {EnemyType::RAT, COLORS::GRAY},
    {EnemyType::BAT, COLORS::GRAY},
    {EnemyType::SNAKE, COLORS::GRAY},
    {EnemyType::VIPER, "#006400"},
    {EnemyType::BAT_LARGE, "#a9a9a9"},
    {EnemyType::GOBLIN, "#00f000"},
    {EnemyType::ORK, "#22aa22"},
    {EnemyType::ORK_BERSERK, "#aa2222"},
    {EnemyType::ORK_BLACK, "#555"},
    {EnemyType::OGRE, "#006400"},
    {EnemyType::I_OGRE, "#cc0044"},
    {EnemyType::PIXI, "#ff1493"},
    {EnemyType::GOBLIN_LIEUTENANT, "#008b8b"},
    {EnemyType::GOBLIN_ROGUE, "#2f4f4f"},
    {EnemyType::ANT, "#8f4f4f"},
    {EnemyType::ANT_QUEEN, "#8f6f4f"},

    {EnemyType::SKELETON, COLORS::WHITE},
    {EnemyType::ZOMBIE, "#8A9A5B"},
    {EnemyType::WRAITH, COLORS::GRAY},

    {EnemyType::FIRE_ELEMENTAL, "#cc1111"},
    {EnemyType::FROST_ELEMENTAL, COLORS::LIGHT_BLUE},
    {EnemyType::ACID_ELEMENTAL, "#11cc11"},

    {EnemyType::STONE_GOLEM, COLORS::GRAY},
    {EnemyType::STEEL_GOLEM, "#2f4f4f"},

    {EnemyType::FIRE_DRAGON, "#cc1111"},
    {EnemyType::FROST_DRAGON, COLORS::LIGHT_BLUE},
    {EnemyType::ACID_DRAGON, "#11cc11"},
};

const std::map<CellSpec, std::string> cellSigns = {
    {CellType::FLOOR, "⋅"s},       {CellType::WALL, "#"s},
    {CellType::UNKNOWN, " "s},     {CellType::DOWNSTAIRS, "&gt;"s},
    {CellType::UPSTAIRS, "&lt;"s}, {CellType::WATER, "="s},
    {CellType::VOID, "⌄"s},
};

const std::map<CellSpec, std::map<bool, std::string>> cellColors = {
    {CellType::FLOOR, {{false, "#555"}, {true, "#2f2f2f"}}},
    {CellType::WALL, {{false, "#aaa"}, {true, "#555"}}},
    {CellType::DOWNSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
    {CellType::UPSTAIRS, {{false, "#aaa"}, {true, "#666"}}},
    {CellType::WATER, {{false, "#5e589e"}, {true, "#557"}}},
    {CellType::VOID, {{false, "#555"}, {true, "#555"}}}};

const std::map<LightType, std::string> lightColors = {
    {LightType::FIRE, "#cc5511"},
    {LightType::MAGIC, "#1133cc"},
    {LightType::ACID, "#11cc11"},
    {LightType::FROST, "#3333ff"},
};

const std::map<CellSpec, std::string> cellWeights = {
    {CellType::FLOOR, "normal"},    {CellType::WALL, "bold"},
    {CellType::DOWNSTAIRS, "bold"}, {CellType::UPSTAIRS, "bold"},
    {CellType::WATER, "normal"},    {CellType::VOID, "normal"},
};

const std::map<CellFeature, std::string> featureColors = {
    {CellFeature::CAVE, "#897546"},  {CellFeature::BLOOD, "#8b0000"},
    {CellFeature::FROST, "#aaaaff"}, {CellFeature::MARK1, "#0000ff"},
    {CellFeature::MARK2, "#00ff00"},
};
