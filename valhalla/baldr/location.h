#ifndef VALHALLA_BALDR_LOCATION_H_
#define VALHALLA_BALDR_LOCATION_H_

#include <valhalla/midgard/pointll.h>
#include <valhalla/proto/common.pb.h>


#include <cstdint>
#include <optional>
#include <string>
#include <vector>


namespace valhalla {
namespace baldr {

/**
 * Input from the outside world to be used in determining where in the graph
 * the route needs to go. A start, middle, destination or via point
 * through which the route must pass
 *
 * TODO: deprecate this struct in favor of protobuf valhalla::Location.
 *
 * @author  Kevin Kreiser
 */
struct Location {
public:
  /**
   * What kind of location this, determines whether a route can double back or not
   * to find the most efficient path
   */
  enum class StopType : uint8_t { BREAK, THROUGH, VIA, BREAK_THROUGH };

  enum class PreferredSide : uint8_t { EITHER, SAME, OPPOSITE };

  /**
   * Optional filters supplied in the request.
   *
   * NOTE: this struct must be kept in sync with the protobuf defined
   * valhalla::Location::SearchFilter in common.proto.
   */
  struct SearchFilter {
  public:
    SearchFilter(valhalla::RoadClass min_road_class = valhalla::RoadClass::kServiceOther,
                 valhalla::RoadClass max_road_class = valhalla::RoadClass::kMotorway,
                 bool exclude_tunnel = false,
                 bool exclude_bridge = false,
                 bool exclude_toll_ = false,
                 bool exclude_ramp = false,
                 bool exclude_ferry_ = false,
                 bool exclude_closures = true,
                 float level = kMaxLevel);

    valhalla::RoadClass min_road_class_;
    valhalla::RoadClass max_road_class_;
    bool exclude_tunnel_;
    bool exclude_bridge_;
    bool exclude_toll_;
    bool exclude_ramp_;
    bool exclude_ferry_;
    bool exclude_closures_;
    float level_;

  protected:
  };

  /**
   * You have to initialize the location with something
   */
  Location() = delete;

  /**
   * Constructor.
   * @param  latlng  the polar coordinates of the location
   */
  Location(const midgard::PointLL& latlng,
           const StopType& stoptype = StopType::BREAK,
           unsigned int min_outbound_reach = 0,
           unsigned int min_inbound_reach = 0,
           unsigned long radius = 0,
           const PreferredSide& side = PreferredSide::EITHER,
           valhalla::RoadClass street_side_cutoff = valhalla::RoadClass::kServiceOther,
           const SearchFilter& search_filter = SearchFilter(),
           std::optional<int8_t> preferred_layer = {});

  /**
   * equality.
   *
   */
  bool operator==(const Location& o) const;

  // coordinates of the location as used for searching the graph
  midgard::PointLL latlng_;
  // type of location for routing
  StopType stoptype_;

  // TODO: this will probably need refactored due to it being very US centered..
  // address of the location, probably should be its own more broken up structure
  std::string name_;
  std::string street_;

  std::optional<std::string> date_time_;
  std::optional<float> heading_;

  // try to find candidates who are reachable from/to this many or more nodes
  // if a given candidate edge is reachable to/from less than this number of nodes its considered to
  // be a disconnected island and we'll search for more candidates until we find at least one that
  // isnt considered a disconnected island
  unsigned int min_outbound_reach_;
  unsigned int min_inbound_reach_;
  // dont return results further away than this (meters) unless there is nothing this close
  unsigned long radius_;

  // which side of the street wrt your input location to leave/arrive from/at
  PreferredSide preferred_side_;
  float node_snap_tolerance_;
  float heading_tolerance_;
  float search_cutoff_;
  float street_side_tolerance_;
  float street_side_max_distance_;
  valhalla::RoadClass street_side_cutoff_;
  SearchFilter search_filter_;

  // coordinates of the location as used for altering the side of street
  std::optional<midgard::PointLL> display_latlng_;

  std::optional<int8_t> preferred_layer_;

  //edge_id in locate request #3412 - https://github.com/valhalla/valhalla/issues/3412
  // Optional list of explicit edge IDs (raw uint64 GraphId values) for this location.
  // When non-empty, Loki's Search::search will skip the spatial bin search and directly
  // construct a PathLocation using these edges. 
  // Populated from the proto Location.path_edges field by PathLocation::fromPBF().
  std::vector<uint64_t> path_edges_;
  
protected:
};
namespace valhalla {
namespace baldr {

inline bool operator==(const LatLng& a, const LatLng& b) {
  return a.has_lat_case() == b.has_lat_case() && a.lat() == b.lat() &&
         a.has_lng_case() == b.has_lng_case() && a.lng() == b.lng();
}

inline bool operator==(const SearchFilter& a, const SearchFilter& b) {
  return a.has_min_road_class_case() == b.has_min_road_class_case() &&
         a.min_road_class() == b.min_road_class() &&
         a.has_max_road_class_case() == b.has_max_road_class_case() &&
         a.max_road_class() == b.max_road_class() && a.exclude_tunnel() == b.exclude_tunnel() &&
         a.exclude_bridge() == b.exclude_bridge() && a.exclude_ramp() == b.exclude_ramp() &&
         a.has_exclude_closures_case() == b.has_exclude_closures_case() &&
         a.exclude_closures() == b.exclude_closures() && a.exclude_toll() == b.exclude_toll() &&
         a.exclude_ferry() == b.exclude_ferry() && a.has_level_case() == b.has_level_case() &&
         a.level() == b.level();
}
inline bool operator==(const Location& a, const Location& b) {
  return a.ll() == b.ll() && a.type() == b.type() && a.has_heading_case() == b.has_heading_case() &&
         a.heading() == b.heading() && a.name() == b.name() && a.street() == b.street() &&
         a.date_time() == b.date_time() && a.side_of_street() == b.side_of_street() &&
         a.has_heading_tolerance_case() == b.has_heading_tolerance_case() &&
         a.heading_tolerance() == b.heading_tolerance() &&
         a.has_node_snap_tolerance_case() == b.has_node_snap_tolerance_case() &&
         a.node_snap_tolerance() == b.node_snap_tolerance() &&
         a.has_minimum_reachability_case() == b.has_minimum_reachability_case() &&
         a.minimum_reachability() == b.minimum_reachability() &&
         a.has_radius_case() == b.has_radius_case() && a.radius() == b.radius() &&
         a.has_accuracy_case() == b.has_accuracy_case() && a.accuracy() == b.accuracy() &&
         a.has_time_case() == b.has_time_case() && a.time() == b.time() &&
         a.skip_ranking_candidates() == b.skip_ranking_candidates() &&
         a.preferred_side() == b.preferred_side() && a.display_ll().lat() == b.display_ll().lat() &&
         a.display_ll() == b.display_ll() &&
         a.has_search_cutoff_case() == b.has_search_cutoff_case() &&
         a.search_cutoff() == b.search_cutoff() &&
         a.has_street_side_tolerance_case() == b.has_street_side_tolerance_case() &&
         a.street_side_tolerance() == b.street_side_tolerance() &&
         a.search_filter() == b.search_filter() &&
         a.has_street_side_max_distance_case() == b.has_street_side_max_distance_case() &&
         a.street_side_max_distance() == b.street_side_max_distance() &&
         a.has_preferred_layer_case() == b.has_preferred_layer_case() &&
         a.preferred_layer() == b.preferred_layer() && a.waiting_secs() == b.waiting_secs() &&
         a.has_street_side_cutoff_case() == b.has_street_side_cutoff_case() &&
         a.street_side_cutoff() == b.street_side_cutoff() &&
         a.has_minimum_inbound_reachability_case() == b.has_minimum_inbound_reachability_case() &&
         a.minimum_inbound_reachability() == b.minimum_inbound_reachability() &&
         a.has_minimum_outbound_reachability_case() == b.has_minimum_outbound_reachability_case() &&
         a.minimum_outbound_reachability() == b.minimum_outbound_reachability();
}
} // namespace baldr
} // namespace valhalla
namespace std {

template <> struct hash<valhalla::Location> {
  size_t operator()(const valhalla::Location& location) const {
    return std::hash<valhalla::midgard::PointLL>()({location.ll().lng(), location.ll().lat()});
  }
};
} // namespace std

#endif // VALHALLA_BALDR_LOCATION_H_
