#include "lss/game/terrain.hpp"
#include "lss/state.hpp"

const std::map<TerrainSpec, std::string> terrainSigns = {
    {TerrainType::TORCH_STAND, "*"},
    {TerrainType::FIREBALL, "*"},
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
    {TerrainType::MAGIC_LIGHT, COLORS::LIGHT_BLUE},
    {TerrainType::MAGIC_LIGHT_LONG, COLORS::LIGHT_BLUE},
    {TerrainType::MAGIC_LIGHT_FOREVER, COLORS::LIGHT_BLUE},
    {TerrainType::ACID_LIGHT_FOREVER, "#11cc11"},
    {TerrainType::STATUE, "#f0f0ff"},
    {TerrainType::ALTAR, COLORS::GRAY},
    {TerrainType::BUSH, "#228b22"},
};

const std::map<ItemSpec, std::string> itemSigns = {
    {ItemType::CORPSE, "%"},        {ItemType::ROCK, "*"},
    {ItemType::PICK_AXE, "("},      {ItemType::SWORD, "("},
    {ItemType::GOLD_RING, "="},     {ItemType::GOLD_COINS, "$"},
    {ItemType::DAGGER, "("},        {ItemType::TORCH, "]"},
    {ItemType::GRASS, "\""},

    {ItemType::GREAT_AXE, "("},     {ItemType::HELMET, "["},
    {ItemType::SHIELD, "["},        {ItemType::GREAVES, "["},
    {ItemType::BOOTS, "["},         {ItemType::PLATE, "["},
    {ItemType::LEATHER_ARMOR, "["}, {ItemType::POTION, "!"},
    {ItemType::SCROLL, "?"},        {ItemType::BONES, "%"},
    {ItemType::THROWING_KNIVES, "↑"}
};

const std::map<ItemSpec, std::string> itemColors = {
    {ItemType::BONES, "#aaa"},

    {ItemType::CORPSE, "#ee1111"},
    {ItemType::ROCK, COLORS::GRAY},
    {ItemType::PICK_AXE, COLORS::WHITE},
    {ItemType::SWORD, "#F7CA88"},
    {ItemType::GOLD_RING, "#ffd700"},
    {ItemType::GOLD_COINS, "#ffd700"},
    {ItemType::DAGGER, COLORS::GRAY},
    {ItemType::TORCH, "#ffa500"},
    {ItemType::GRASS, "#006400"},

    {ItemType::GREAT_AXE, COLORS::WHITE},
    {ItemType::HELMET, COLORS::WHITE},
    {ItemType::SHIELD, COLORS::WHITE},
    {ItemType::GREAVES, COLORS::WHITE},
    {ItemType::BOOTS, COLORS::WHITE},
    {ItemType::PLATE, COLORS::WHITE},
    {ItemType::LEATHER_ARMOR, COLORS::WHITE},
    {ItemType::POTION, COLORS::LIGHT_BLUE},
    {ItemType::SCROLL, COLORS::WHITE},
    {ItemType::THROWING_KNIVES, COLORS::WHITE}
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
    {EnemyType::PIXI, "p"},
    {EnemyType::OGRE, "O"},
    {EnemyType::GOBLIN_LIEUTENANT, "g"},
    {EnemyType::GOBLIN_ROGUE, "g"},
};

const std::map<EnemySpec, std::string> enemyColors = {
    {EnemyType::RAT, COLORS::GRAY},
    {EnemyType::BAT, COLORS::GRAY},
    {EnemyType::SNAKE, COLORS::GRAY},
    {EnemyType::VIPER, "#006400"},
    {EnemyType::BAT_LARGE, "#a9a9a9"},
    {EnemyType::GOBLIN, "#00f000"},
    {EnemyType::ORK, "#22aa22"},
    {EnemyType::ORK_BLACK, "#555"},
    {EnemyType::OGRE, "#006400"},
    {EnemyType::PIXI, "#ff1493"},
    {EnemyType::GOBLIN_LIEUTENANT, "#008b8b"},
    {EnemyType::GOBLIN_ROGUE, "#2f4f4f"},
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
};

const std::map<CellSpec, std::string> cellWeights = {
    {CellType::FLOOR, "normal"},    {CellType::WALL, "bold"},
    {CellType::DOWNSTAIRS, "bold"}, {CellType::UPSTAIRS, "bold"},
    {CellType::WATER, "normal"},    {CellType::VOID, "normal"},
};

const std::map<CellFeature, std::string> featureColors = {
    {CellFeature::CAVE, "#897546"},
    {CellFeature::BLOOD, "#8b0000"},
    {CellFeature::MARK1, "#0000ff"},
    {CellFeature::MARK2, "#00ff00"},
};
