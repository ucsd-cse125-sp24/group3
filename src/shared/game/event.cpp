#include "shared/game/event.hpp"

std::ostream& operator<<(std::ostream& os, const Event& evt) {
    os << "Event { .evt_source=" << evt.evt_source
        << " .type=" << evt.type << "}";
    return os;
}

#define TO_STR(type) \
    case EventType::type: \
        os << #type; \
        break

std::ostream& operator<<(std::ostream& os, const EventType& type) {
    switch (type) {
        TO_STR(LobbyAction);
        TO_STR(LoadGameState);
        TO_STR(VerticalKeyDown);
        TO_STR(HorizontalKeyDown);
        TO_STR(VerticalKeyUp);
        TO_STR(HorizontalKeyUp);
        TO_STR(Jump);
        TO_STR(MoveAbsolute);
        TO_STR(SpawnEntity);
    default:
        os << "Unknown EventType";
        break;
    }
    return os;
}