import re

# Load your decoded binary data
with open("data.bin", "rb") as f:
    data = f.read()

# Find all occurrences of the "OCTM" header
# Note: This is a basic split. The actual exocad format prefixes the CTM data 
# with the byte length of the CTM block, which is a safer way to parse it.
chunks = data.split(b'OCTM')

# Skip the first chunk (it's the data before the first 3D model)
for i in range(1, len(chunks)):
    # Re-add the OCTM header that was removed by the split
    ctm_data = b'OCTM' + chunks[i]
    
    # Save each 3D model as a .ctm file
    with open(f"extracted_model_{i}.ctm", "wb") as out_file:
        out_file.write(ctm_data)
        
print(f"Extracted {len(chunks)-1} CTM files.")