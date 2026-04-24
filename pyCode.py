import math
PLATFORMS = {
    "Hibernia": {"lat": 43.7504, "lon": -48.7819, "depth": 78},
    "Sea Rose": {"lat": 46.7895, "lon": -48.1417, "depth": 107},
    "Terra Nova": {"lat": 46.4000, "lon": -48.4000, "depth": 91},
    "Hebron": {"lat": 46.5440, "lon": -48.4980, "depth": 93}
}

def calculate_cross_track_distance(ice_lat, ice_lon, heading, plat_lat, plat_lon):
    y = (plat_lat - ice_lat) * 60.0 
    x = (plat_lon - ice_lon) * 60.0 * math.cos(math.radians(ice_lat))
    rad_h = math.radians(heading)
    distance = abs(x * math.cos(rad_h) - y * math.sin(rad_h))
    return distance
def assess_threats(ice_lat, ice_lon, heading, keel_depth):
    print("\n" + "="*50)
    print(f"ICEBERG DATA -> Lat: {ice_lat}, Lon: {ice_lon}, Head: {heading}°, Keel: {keel_depth}m")
    print("="*50)
    
    for name, data in PLATFORMS.items():
        print(f"\n--- {name} ---")
        p_depth = data["depth"]
        if keel_depth >= p_depth * 1.10:
            print(f"Status: GROUNDED (Keel {keel_depth}m >= 110% of {p_depth}m depth)")
            print("Surface Threat: GREEN")
            print("Subsea Threat:  GREEN")
            continue
        dist_nm = calculate_cross_track_distance(ice_lat, ice_lon, heading, data["lat"], data["lon"])
        print(f"Closest Approach: {dist_nm:.2f} nautical miles")
        if dist_nm < 5:
            surf_threat = "RED"
        elif 5 <= dist_nm <= 10:
            surf_threat = "YELLOW"
        else:
            surf_threat = "GREEN"
            
        print(f"Surface Threat: {surf_threat}")
        
        if dist_nm <= 25:
            ratio = (keel_depth / p_depth) * 100
            print(f"Subsea Ratio: {ratio:.1f}% of water depth")
            
            if 90 <= ratio < 110:
                sub_threat = "RED"
            elif 70 <= ratio < 90:
                sub_threat = "YELLOW"
            else:
                sub_threat = "GREEN"
            print(f"Subsea Threat:  {sub_threat}")
        else:
            print("Subsea Threat:  N/A (Passes > 25nm away)")

if __name__ == "__main__":
    JUDGE_LAT = 46.2000
    JUDGE_LON = -48.6000
    JUDGE_HEADING = 45
    JUDGE_KEEL = 85.0
    
    assess_threats(JUDGE_LAT, JUDGE_LON, JUDGE_HEADING, JUDGE_KEEL)