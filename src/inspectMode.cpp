#include "lss/ui/inspectMode.hpp"
#include "lss/LSSApp.hpp"
#include "lss/fragment.hpp"
#include "lss/generator/room.hpp"
#include "lss/utils.hpp"

auto F = [](std::string c) { return std::make_shared<Fragment>(c); };

bool InspectMode::processKey(KeyEvent event) {
  switch (event.getCode()) {
  case SDL_SCANCODE_J:
  case SDL_SCANCODE_H:
  case SDL_SCANCODE_L:
  case SDL_SCANCODE_K:
  case SDL_SCANCODE_Y:
  case SDL_SCANCODE_U:
  case SDL_SCANCODE_B:
  case SDL_SCANCODE_N: {
    auto d = utils::getDir(event.getCode());
    if (d == std::nullopt)
      break;
    auto nc = app->hero->currentLocation->getCell(
        app->hero->currentLocation
            ->cells[app->state->cursor.y][app->state->cursor.x],
        *utils::getDirectionByName(*d));
    if (!nc)
      break;
    app->state->invalidateSelection("move inspect cursor");
    app->state->cursor = {(*nc)->x, (*nc)->y};
    render();
    return true;
  } break;
  }
  return false;
}

void InspectMode::render() {
  auto location = app->hero->currentLocation;
  auto cell = location->cells[app->state->cursor.y][app->state->cursor.x];
  auto objects = location->getObjects(cell);
  auto cc = app->hero->currentCell;
  auto check = "<span color='green'>✔</span>";
  app->state->selection.clear();

  auto line = location->getLine(app->hero->currentCell, cell);
  for (auto c : line) {
    app->state->selection.push_back({{c->x, c->y}, "#33f"});
  }

  app->inspectState->setContent(
      {F(fmt::format("Selected cell: <b>{}.{}</b>", cell->x, cell->y))});
  app->inspectState->appendContent(State::END_LINE);
  auto vs = "UNKNOWN";
  if (cell->visibilityState == VisibilityState::SEEN) {
    vs = "SEEN";
  } else if (cell->visibilityState == VisibilityState::VISIBLE) {
    vs = "VISISBLE";
  }
  app->inspectState->setContent(
      {F(fmt::format("Visibility state: <b>{}</b>", vs))});
  app->inspectState->appendContent(State::END_LINE);

  if (!app->debug && !app->hero->canSee(cell)) {
    app->inspectState->appendContent(
        {F(fmt::format("You cannot see this cell"))});
    app->inspectState->appendContent(State::END_LINE);
    return;
  }

  app->inspectState->appendContent(
      {F(fmt::format("Type: <b>{}</b>", cell->type.name))});
  app->inspectState->appendContent(State::END_LINE);
  if (cell->type == CellType::UNKNOWN)
    return;
  app->inspectState->appendContent(
      {F(fmt::format("Type <b>PASS</b>THROUGH: [<b>{}</b>]",
                     cell->type.passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(
      {F(fmt::format("Type <b>SEE</b>THROUGH: [<b>{}</b>]",
                     cell->type.passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "<b>PASS</b>THROUGH: [<b>{}</b>]", cell->passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "<b>SEE</b>THROUGH: [<b>{}</b>]", cell->passThrough ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "Illuminated: [<b>{}</b>]", cell->illuminated ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(
      {F(fmt::format("Illumination: <b>{}</b>", cell->illumination))});
  app->inspectState->appendContent(State::END_LINE);
  auto f = app->state->fragments[cell->y * app->state->width + cell->x];
  // app->inspectState->appendContent({F(fmt::format(
  //     "Cell Illumination: <b>{}</b>", f->alpha))});
  // app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(
      fmt::format("Cell features count: <b>{}</b>", cell->features.size()))});
  app->inspectState->appendContent(State::END_LINE);
  if (cell->features.size() > 0) {
    app->inspectState->appendContent({F("<b>Cell Features</b>")});
    app->inspectState->appendContent(State::END_LINE);
  }
  for (auto f : cell->features) {
    if (f == CellFeature::BLOOD) {
      app->inspectState->appendContent(
          {F(fmt::format("BLOOD: [<b>{}</b>]", check))});
    }
    if (f == CellFeature::CAVE) {
      app->inspectState->appendContent(
          {F(fmt::format("CAVE: [<b>{}</b>]", check))});
    }
    if (f == CellFeature::ACID) {
      app->inspectState->appendContent(
          {F(fmt::format("ACID: [<b>{}</b>]", check))});
    }
    if (f == CellFeature::FROST) {
      app->inspectState->appendContent(
          {F(fmt::format("FROST: [<b>{}</b>]", check))});
    }
    app->inspectState->appendContent(State::END_LINE);
  }
  app->inspectState->appendContent(State::END_LINE);
  if (cell->room == nullptr) {
    app->inspectState->appendContent(
        {F(fmt::format("<b>Room is nullptr!</b>"))});
    app->inspectState->appendContent(State::END_LINE);
  } else {
    app->inspectState->appendContent(
        {F(fmt::format("Room @ <b>{}.{} [{}x{}]</b>", cell->room->x,
                       cell->room->y, cell->room->width, cell->room->height))});
    app->inspectState->appendContent(State::END_LINE);
    app->inspectState->appendContent(
        {F(fmt::format("HALL: [<b>{}</b>]",
                       cell->room->type == RoomType::HALL ? check : " "))});
    app->inspectState->appendContent(State::END_LINE);
    app->inspectState->appendContent(
        {F(fmt::format("PASSAGE: [<b>{}</b>]",
                       cell->room->type == RoomType::PASSAGE ? check : " "))});
    app->inspectState->appendContent(State::END_LINE);
    app->inspectState->appendContent({F(fmt::format(
        "Room features count: <b>{}</b>", cell->room->features.size()))});
    app->inspectState->appendContent(State::END_LINE);
    if (app->debug) {
      for (auto c : cell->room->cells) {
        app->state->selection.push_back({{c->x, c->y}, "#811"});
      }
    }

    if (cell->room->features.size() > 0) {
      app->inspectState->appendContent({F("<b>Room Features</b>")});
      app->inspectState->appendContent(State::END_LINE);
    }
    for (auto f : cell->room->features) {
      if (f == RoomFeature::DUNGEON) {
        app->inspectState->appendContent(
            {F(fmt::format("DUNGEON: [<b>{}</b>]", check))});
      }
      if (f == RoomFeature::CAVE) {
        app->inspectState->appendContent(
            {F(fmt::format("CAVE: [<b>{}</b>]", check))});
      }
      app->inspectState->appendContent(State::END_LINE);
    }
    app->inspectState->appendContent(State::END_LINE);
  }

  app->inspectState->appendContent(
      {F(fmt::format("Light sources: <b>{}</b>", cell->lightSources.size()))});

  for (auto ls : cell->lightSources) {
    auto c = ls->currentCell;
    app->state->selection.push_back({{c->x, c->y}, "#1f1"});
  }
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(fmt::format(
      "Hero: [<b>{}</b>]", cell == app->hero->currentCell ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent(
      {F(fmt::format("Hero can <b>pass</b>: [<b>{}</b>]",
                     cell->canPass(app->hero->traits) ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);

  app->inspectState->appendContent(
      {F(fmt::format("Hero can <b>see</b>: [<b>{}</b>]",
                     app->hero->canSee(cell) ? check : " "))});
  app->inspectState->appendContent(State::END_LINE);
  app->inspectState->appendContent({F(
      fmt::format("Distance to hero: <b>{}</b>",
                  sqrt(pow(cc->x - cell->x, 2) + pow(cc->y - cell->y, 2))))});
  app->inspectState->appendContent(State::END_LINE);

  if (cell->type == CellType::WALL)
    return;

  auto allEnemies = utils::castObjects<Enemy>(location->objects);
  for (auto e : allEnemies) {
    if (e->canSee(cell)) {
      app->inspectState->appendContent(
          {F(fmt::format("<b>{} @ {}.{}</b> can see: [<b>{}</b>]", e->type.name,
                         e->currentCell->x, e->currentCell->y,
                         e->canSee(cell) ? check : " "))});
      app->state->selection.push_back(
          {{e->currentCell->x, e->currentCell->y}, "#fff"});
      app->inspectState->appendContent(State::END_LINE);
    }
  }

  app->inspectState->appendContent(
      {F(fmt::format("Objects count: <b>{}</b>", objects.size()))});
  app->inspectState->appendContent(State::END_LINE);

  if (objects.size() > 0) {
    auto isDoor = utils::castObjects<Door>(objects).size() > 0;
    app->inspectState->appendContent(
        {F(fmt::format("Door: [<b>{}</b>]", isDoor ? check : " "))});
    app->inspectState->appendContent(State::END_LINE);
    // auto isTorch = utils::castObjects<TorchStand>(objects).size() > 0;
    // app->inspectState->appendContent(
    //     {F(fmt::format("Torch: [<b>{}</b>]", isTorch ? check : " "))});
    // app->inspectState->appendContent(State::END_LINE);
    // auto isStatue = utils::castObjects<Statue>(objects).size() > 0;
    // app->inspectState->appendContent(
    //     {F(fmt::format("Statue: [<b>{}</b>]", isStatue ? check : " "))});
    // app->inspectState->appendContent(State::END_LINE);
    auto enemies = utils::castObjects<Enemy>(objects);
    if (enemies.size() > 0) {
      std::vector<std::string> enemyNames;
      for (auto e : enemies) {
        enemyNames.push_back(e->type.name);
        app->inspectState->appendContent(
            {F(fmt::format("<b>{} @ {}.{}</b> can see HERO: [<b>{}</b>]",
                           e->type.name, e->currentCell->x, e->currentCell->y,
                           e->canSee(cc) ? check : " "))});
        app->inspectState->appendContent(State::END_LINE);

        app->inspectState->appendContent({F(fmt::format(
            "Has glow: [<b>{}</b>]", e->hasLight() ? check : " "))});
        app->inspectState->appendContent(State::END_LINE);
        auto glow = e->getGlow();
        if (glow) {
          app->inspectState->appendContent(
              {F(fmt::format("Light: <b>{}</b>, stable: {}", (*glow).distance,
                             (*glow).stable))});
          app->inspectState->appendContent(State::END_LINE);
        }
        app->inspectState->appendContent({F("Traits:")});
        app->inspectState->appendContent(State::END_LINE);
        for (auto t : e->traits) {
          app->inspectState->appendContent(
              {F(fmt::format("    * {}", t.name))});
          app->inspectState->appendContent(State::END_LINE);
        }

        app->inspectState->appendContent({F("Inventory:")});
        app->inspectState->appendContent(State::END_LINE);
        for (auto s : e->equipment->slots) {
          app->inspectState->appendContent({F(fmt::format(
              "    * {} -- {}", s->name,
              s->item != nullptr ? s->item->getTitle() : "empty"))});
          app->inspectState->appendContent(State::END_LINE);
        }
      }
      app->inspectState->appendContent({F(
          fmt::format("Enemies: <b>{}</b>", LibLog::utils::join(enemyNames, ", ")))});
      app->inspectState->appendContent(State::END_LINE);
    }
    auto items = utils::castObjects<Item>(objects);
    if (items.size() > 0) {
      std::vector<std::string> itemNames;
      for (auto i : items) {
        itemNames.push_back(i->getFullTitle());
      }
      app->inspectState->appendContent(
          {F(fmt::format("Items: <b>{}</b>", LibLog::utils::join(itemNames, ", ")))});
      app->inspectState->appendContent(State::END_LINE);
    }
  }
  app->state->invalidateSelection("move inspect cursor");
}
