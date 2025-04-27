import os
from PIL import Image, ImageDraw
import numpy as np
import json

# CFG
ROOMS_FOLDER = "rooms"
RESULT_FOLDER = "result"
TARGET_COLOR = (37, 54, 68)
TOLERANCE = 10
MIN_WALL_LENGTH = 30
MIN_WALL_THICKNESS = 10

os.makedirs(RESULT_FOLDER, exist_ok=True)
all_rooms = []

for filename in os.listdir(ROOMS_FOLDER):
    if not filename.endswith(".png"):
        continue

    room_id = filename[:-4]
    room_name = room_id
    room_type = "hall" if "hallway" in room_id.lower() else "room"

    # Load image
    image_path = os.path.join(ROOMS_FOLDER, filename)
    image = Image.open(image_path).convert('RGB')
    width, height = image.size
    center_x = width // 2
    center_y = height // 2
    data = np.array(image)

    # Create mask
    mask = np.all(np.abs(data - TARGET_COLOR) <= TOLERANCE, axis=-1)
    used = np.zeros_like(mask, dtype=bool)
    walls = []

    # Horizontal detection
    for y in range(height):
        x = 0
        while x < width:
            if mask[y, x] and not used[y, x]:
                start_x = x
                while x < width and mask[y, x] and not used[y, x]:
                    used[y, x] = True
                    x += 1
                wall_length = x - start_x
                if wall_length > 2:
                    walls.append({
                        "coords": {"X": start_x, "Y": y},
                        "l": wall_length,
                        "h": 0,
                        "w": 1
                    })
            else:
                x += 1

    # Vertical detection
    for x in range(width):
        y = 0
        while y < height:
            if mask[y, x] and not used[y, x]:
                start_y = y
                while y < height and mask[y, x] and not used[y, x]:
                    used[y, x] = True
                    y += 1
                wall_length = y - start_y
                if wall_length > 2:
                    walls.append({
                        "coords": {"X": x, "Y": start_y},
                        "l": wall_length,
                        "h": 90,
                        "w": 1
                    })
            else:
                y += 1

    # Merge adjacent walls
    merged_walls = []
    walls.sort(key=lambda w: (w["h"], w["coords"]["X"], w["coords"]["Y"]))
    for wall in walls:
        if not merged_walls:
            merged_walls.append(wall)
            continue

        last = merged_walls[-1]
        if (
            wall["h"] == last["h"]
            and wall["l"] == last["l"]
            and wall["coords"]["X"] == last["coords"]["X"]
            and wall["coords"]["Y"] == last["coords"]["Y"] + last["w"]
        ):
            last["w"] += wall["w"]
        else:
            merged_walls.append(wall)

    # Filter small walls
    filtered_walls = []
    for wall in merged_walls:
        if wall["l"] >= MIN_WALL_LENGTH and wall["w"] >= MIN_WALL_THICKNESS:
            filtered_walls.append(wall)

    # Build room data
    room_json = {
        "id": room_id,
        "name": room_name,
        "type": room_type,
        "center": {"X": center_x, "Y": center_y},
        "walls": filtered_walls,
        "doors": []
    }

    all_rooms.append(room_json)

    # Save individual room JSON
    json_path = os.path.join(RESULT_FOLDER, f"{room_id}.json")
    with open(json_path, 'w') as f:
        json.dump(room_json, f, indent=4)

    # Create debug image
    check_img = Image.new('RGB', (width, height), (255, 255, 255))
    draw = ImageDraw.Draw(check_img)
    for wall in filtered_walls:
        x = wall["coords"]["X"]
        y = wall["coords"]["Y"]
        if wall["h"] == 0:
            draw.rectangle([x, y, x + wall["l"], y + wall["w"]], fill=(37, 54, 68))
        else:
            draw.rectangle([x, y, x + wall["w"], y + wall["l"]], fill=(37, 54, 68))

    debug_img_path = os.path.join(RESULT_FOLDER, f"{room_id}_debug.png")
    check_img.save(debug_img_path)

    print(f"Processed {filename}")

# Create world.json
world_json = {
    "rooms": sorted(all_rooms, key=lambda r: r["name"].lower())
}

with open('world.json', 'w') as f:
    json.dump(world_json, f, indent=4)

print("All rooms processed and world.json created")
