# Given a directory of image file (.jpg, .png and/or .pcx), convert them into DamPBNAscii format files.
#
# If the images are larger than 320x200, they will be scaled by a ratio necessary to make them at most that size,
# while preserving the aspect ratio.
# Any images with >64 colors will be quantized to 64 colors with dithering
#
# The tool takes 5 command line arguments; an input directory, an output directory, a path to a CSV file
# containing information about each of the images, an argument that specifies the kind of compression to be used,
# and an argument specifing whether files should be created using the v2 format (with transparency)

# About the CSV file:
#
# The CSV file should contain a series of lines, each with 3 values:
#  - The name of the file
#  - A description of the image (will be truncated to 32 characters)
#  - A category ID
#
# Any images without these entries in the CSV file will be given an uncategorized ID and name of 'Default Image'.
#
import sys, os, PIL.Image, PIL.ImagePalette

# If True, also writes a copy of the converted image as a PNG for inspection
debug_output = True
 
# Each entry - [ASCII_character, FG, BG, R, G, B]
# This chart contains a list of RGB values and the ASCII character, FG and 
# background attribute that approximates that color (if you squint, I suppose).
#
# An image is provided that contains the palette with the RGB values as sorted
# below.  Unlike the original DamPBN, which quantizes down to 64 colors from
# the palette of the image, this tool will convert the image to use the palette
# from the aforementioned image, then quantize down to 64 or fewer colors.
#
# Before the file is saved, a list of the entries in this chart used in the 
# final image will be assembled into final_chart.  The image will then be
# looked over again, and each pixel will be assigned the index of final_chart
# corresponding to that color.  The contents of final_chart will constitute the
# 'palette' for the output file.

palette_base_image="kefka.png"
final_chart = []

color_chart = [
        [219, 0, 0, 0, 0, 0],
        [177, 1, 0, 0, 0, 124],
        [178, 1, 0, 0, 0, 148],
        [219, 1, 0, 0, 0, 170],
        [176, 1, 0, 0, 0, 88],
        [177, 2, 0, 0, 124, 0],
        [177, 3, 0, 0, 124, 124],
        [177, 3, 1, 0, 124, 170],
        [178, 2, 0, 0, 148, 0],
        [178, 3, 0, 0, 148, 148],
        [178, 3, 1, 0, 148, 170],
        [178, 2, 1, 0, 148, 90],
        [219, 2, 0, 0, 170, 0],
        [178, 3, 2, 0, 170, 148],
        [219, 3, 0, 0, 170, 170],
        [176, 3, 2, 0, 170, 88],
        [176, 2, 0, 0, 88, 0],
        [176, 2, 1, 0, 88, 150],
        [176, 3, 1, 0, 88, 170],
        [176, 3, 0, 0, 88, 88],
        [177, 3, 2, 0, 170, 124],
        [178, 8, 7, 115, 115, 115],
        [178, 9, 7, 115, 115, 237],
        [178, 10, 5, 115, 224, 115],
        [178, 10, 4, 115, 224, 74],
        [178, 10, 7, 114, 237, 115],
        [178, 11, 7, 115, 237, 237],
        [178, 8, 5, 115, 74, 115],
        [178, 9, 4, 115, 74, 224],
        [178, 8, 4, 115, 74, 74],
        [178, 9, 6, 115, 85, 224],
        [178, 8, 6, 115, 85, 74],
        [177, 4, 0, 124, 0, 0],
        [177, 5, 0, 124, 0, 124],
        [177, 5, 1, 124, 0, 170],
        [177, 4, 2, 124, 124, 0],
        [177, 7, 0, 124, 124, 124],
        [177, 7, 1, 124, 124, 170],
        [177, 6, 2, 124, 136, 0],
        [177, 6, 3, 124, 136, 124],
        [177, 7, 2, 124, 170, 124],
        [177, 7, 3, 124, 170, 170],
        [177, 6, 0, 124, 60, 0],
        [177, 6, 1, 124, 60, 124],
        [177, 8, 7, 136, 136, 136],
        [176, 14, 1, 136, 136, 155],
        [176, 15, 1, 136, 136, 195],
        [177, 9, 7, 136, 136, 218],
        [176, 14, 0, 136, 136, 42],
        [176, 13, 2, 136, 155, 136],
        [176, 13, 3, 136, 155, 195],
        [176, 12, 2, 136, 155, 42],
        [177, 10, 5, 136, 187, 136],
        [177, 11, 4, 136, 187, 187],
        [177, 11, 5, 136, 187, 218],
        [177, 10, 4, 136, 187, 60],
        [176, 15, 2, 136, 195, 136],
        [177, 11, 6, 136, 195, 187],
        [176, 15, 3, 136, 195, 195],
        [176, 14, 2, 136, 195, 42],
        [177, 10, 6, 136, 195, 60],
        [177, 10, 7, 136, 218, 136],
        [177, 11, 7, 136, 218, 218],
        [176, 13, 0, 136, 42, 136],
        [176, 12, 1, 136, 42, 155],
        [176, 13, 1, 136, 42, 195],
        [176, 12, 0, 136, 42, 42],
        [177, 8, 5, 136, 60, 136],
        [177, 9, 4, 136, 60, 187],
        [177, 9, 5, 136, 60, 218],
        [177, 8, 4, 136, 60, 60],
        [177, 9, 6, 136, 85, 187],
        [177, 8, 6, 136, 85, 60],
        [178, 4, 0, 148, 0, 0],
        [178, 5, 0, 148, 0, 148],
        [178, 5, 1, 148, 0, 170],
        [178, 4, 1, 148, 0, 90],
        [178, 6, 2, 148, 115, 0],
        [178, 6, 3, 148, 115, 90],
        [178, 7, 0, 148, 148, 148],
        [178, 7, 1, 148, 148, 170],
        [178, 7, 2, 148, 170, 148],
        [178, 7, 3, 148, 170, 170],
        [178, 6, 0, 148, 74, 0],
        [178, 6, 1, 148, 74, 90],
        [178, 4, 2, 148, 90, 0],
        [178, 5, 2, 148, 90, 148],
        [178, 5, 3, 148, 90, 170],
        [178, 4, 3, 148, 90, 90],
        [176, 10, 5, 155, 136, 155],
        [176, 11, 5, 155, 136, 195],
        [176, 10, 4, 155, 136, 42],
        [176, 8, 7, 155, 155, 155],
        [176, 9, 7, 155, 155, 195],
        [176, 10, 6, 155, 155, 42],
        [176, 10, 7, 155, 195, 155],
        [176, 11, 7, 155, 195, 195],
        [176, 9, 4, 155, 42, 136],
        [176, 8, 5, 155, 42, 155],
        [176, 9, 5, 155, 42, 195],
        [176, 8, 4, 155, 42, 42],
        [176, 9, 6, 155, 88, 136],
        [176, 8, 6, 155, 88, 42],
        [219, 4, 0, 170, 0, 0],
        [177, 5, 4, 170, 0, 124],
        [178, 5, 4, 170, 0, 148],
        [219, 5, 0, 170, 0, 170],
        [176, 5, 4, 170, 0, 90],
        [176, 7, 6, 170, 115, 92],
        [177, 7, 5, 170, 124, 170],
        [177, 7, 6, 170, 136, 124],
        [177, 7, 4, 170, 148, 148],
        [177, 7, 5, 170, 148, 170],
        [177, 7, 6, 170, 155, 148],
        [219, 7, 0, 170, 170, 170],
        [176, 6, 5, 170, 42, 150],
        [176, 6, 4, 170, 42, 90],
        [177, 6, 4, 170, 60, 0],
        [177, 6, 5, 170, 60, 124],
        [178, 6, 4, 170, 74, 0],
        [178, 6, 5, 170, 74, 90],
        [219, 6, 0, 170, 85, 0],
        [176, 7, 5, 170, 90, 170],
        [176, 7, 4, 170, 90, 90],
        [177, 12, 3, 187, 136, 136],
        [177, 13, 2, 187, 136, 187],
        [177, 13, 3, 187, 136, 218],
        [177, 12, 2, 187, 136, 60],
        [177, 14, 1, 187, 187, 136],
        [177, 15, 0, 187, 187, 187],
        [177, 15, 1, 187, 187, 218],
        [177, 14, 0, 187, 187, 60],
        [177, 14, 3, 187, 218, 136],
        [177, 15, 2, 187, 218, 187],
        [177, 15, 3, 187, 218, 218],
        [177, 14, 2, 187, 218, 60],
        [177, 12, 1, 187, 60, 136],
        [177, 13, 0, 187, 60, 187],
        [177, 13, 1, 187, 60, 218],
        [177, 12, 0, 187, 60, 60],
        [176, 15, 4, 195, 136, 136],
        [176, 14, 5, 195, 136, 155],
        [176, 15, 5, 195, 136, 195],
        [176, 14, 4, 195, 136, 42],
        [176, 15, 6, 195, 155, 136],
        [176, 13, 7, 195, 155, 195],
        [176, 14, 6, 195, 155, 42],
        [176, 14, 7, 195, 195, 155],
        [176, 15, 7, 195, 195, 195],
        [176, 13, 4, 195, 42, 136],
        [176, 12, 5, 195, 42, 155],
        [176, 13, 5, 195, 42, 195],
        [176, 12, 4, 195, 42, 42],
        [176, 13, 6, 195, 88, 136],
        [176, 12, 6, 195, 88, 42],
        [177, 12, 7, 218, 136, 136],
        [177, 13, 7, 218, 136, 218],
        [177, 14, 5, 218, 187, 136],
        [177, 15, 4, 218, 187, 187],
        [177, 15, 5, 218, 187, 218],
        [177, 14, 4, 218, 187, 60],
        [177, 15, 6, 218, 195, 187],
        [177, 14, 6, 218, 195, 60],
        [177, 14, 7, 218, 218, 136],
        [177, 15, 7, 218, 218, 218],
        [177, 12, 5, 218, 60, 136],
        [177, 13, 4, 218, 60, 187],
        [177, 13, 5, 218, 60, 218],
        [177, 12, 4, 218, 60, 60],
        [177, 13, 6, 218, 85, 187],
        [177, 12, 6, 218, 85, 60],
        [178, 12, 3, 224, 115, 115],
        [178, 13, 2, 224, 115, 224],
        [178, 12, 2, 224, 115, 74],
        [178, 14, 1, 224, 224, 115],
        [178, 14, 0, 224, 224, 74],
        [178, 14, 3, 224, 237, 115],
        [178, 15, 3, 224, 237, 237],
        [178, 12, 1, 224, 74, 115],
        [178, 13, 0, 224, 74, 224],
        [178, 13, 1, 224, 74, 237],
        [178, 12, 0, 224, 74, 74],
        [178, 14, 2, 224, 237, 74],
        [178, 12, 7, 237, 115, 115],
        [178, 13, 7, 237, 115, 237],
        [178, 14, 5, 237, 224, 115],
        [178, 14, 4, 237, 224, 74],
        [178, 15, 7, 237, 237, 237],
        [178, 12, 5, 237, 74, 115],
        [178, 13, 5, 237, 74, 237],
        [178, 12, 4, 237, 74, 74],
        [178, 13, 6, 237, 85, 224],
        [178, 12, 6, 237, 85, 74],
        [219, 15, 0, 255, 255, 255],
        [219, 14, 0, 255, 255, 85],
        [219, 13, 0, 255, 85, 255],
        [219, 12, 0, 255, 85, 85],
        [176, 11, 0, 42, 136, 136],
        [176, 11, 1, 42, 136, 195],
        [176, 10, 0, 42, 136, 42],
        [176, 9, 2, 42, 155, 136],
        [176, 8, 3, 42, 155, 155],
        [176, 9, 3, 42, 155, 195],
        [176, 8, 2, 42, 155, 42],
        [176, 11, 2, 42, 195, 136],
        [176, 11, 3, 42, 195, 195],
        [176, 10, 2, 42, 195, 42],
        [176, 9, 0, 42, 42, 136],
        [176, 8, 1, 42, 42, 155],
        [176, 9, 1, 42, 42, 195],
        [176, 8, 0, 42, 42, 42],
        [177, 8, 3, 60, 136, 136],
        [177, 9, 2, 60, 136, 187],
        [177, 9, 3, 60, 136, 218],
        [177, 8, 2, 60, 136, 60],
        [177, 10, 1, 60, 187, 136],
        [177, 11, 0, 60, 187, 187],
        [177, 11, 1, 60, 187, 218],
        [177, 10, 0, 60, 187, 60],
        [177, 10, 3, 60, 218, 136],
        [177, 11, 2, 60, 218, 187],
        [177, 11, 3, 60, 218, 218],
        [177, 10, 2, 60, 218, 60],
        [177, 8, 1, 60, 60, 136],
        [177, 9, 0, 60, 60, 187],
        [177, 9, 1, 60, 60, 218],
        [177, 8, 0, 60, 60, 60],
        [178, 8, 3, 74, 115, 115],
        [178, 9, 2, 74, 115, 224],
        [178, 8, 2, 74, 115, 74],
        [178, 10, 1, 74, 224, 115],
        [178, 11, 1, 74, 224, 237],
        [178, 10, 0, 74, 224, 74],
        [178, 11, 2, 74, 237, 224],
        [178, 10, 2, 74, 237, 74],
        [178, 8, 1, 74, 74, 115],
        [178, 9, 0, 74, 74, 224],
        [178, 8, 0, 74, 74, 74],
        [219, 11, 0, 85, 255, 255],
        [219, 10, 0, 85, 255, 85],
        [219, 9, 0, 85, 85, 255],
        [219, 8, 0, 85, 85, 85],
        [176, 4, 0, 88, 0, 0],
        [176, 4, 1, 88, 0, 150],
        [176, 5, 1, 88, 0, 170],
        [176, 5, 0, 88, 0, 88],
        [176, 4, 2, 88, 150, 0],
        [176, 4, 3, 88, 150, 150],
        [176, 5, 3, 88, 150, 170],
        [176, 5, 2, 88, 150, 88],
        [176, 7, 3, 88, 170, 170],
        [176, 7, 2, 88, 170, 88],
        [176, 6, 0, 88, 42, 0],
        [176, 6, 1, 88, 42, 150],
        [176, 7, 1, 88, 88, 170],
        [176, 7, 0, 88, 88, 88]
    ]

# Converts the RGB triplet into an ASCII character, FG and BG color based on 
# the color_chart table above.
def create_ascii_palette(pal, num_colors):
    processed_pal = []
    for i in range(num_colors):
        found = False
        for j in color_chart:
            if pal[3*i] == j[3] and pal[3*i+1] == j[4] and pal[3*i+2] == j[5]:
                processed_pal.append(j[0])
                processed_pal.append(j[1])
                processed_pal.append(j[2])
                found = True
        if found == False:
            print(f"Warning! Not found! {pal[3*i]}, {pal[3*i+1]}, {pal[3*i+2]}")
            
    for i in range(192-len(processed_pal)):
        processed_pal.append(0)
        
    return processed_pal

def resize(input_file):
    # Get the width and height of the image
    width, height = input_file.size

    ratio = 1.0

    # If the width of the image is greater than 160, calculate the ratio to scale it down to 160
    if width > 160:
        ratio = 160.0 / width

    # If the resulting image is still too tall, calculate the ratio to scale it down to 100
    if height * ratio > 100:
        ratio = 100.0 / height

    # Calculate the new width and height
    new_width = int(width * ratio)
    new_height = int(height * ratio)

    # Resize the image
    if ratio != 1.0:
        input_file = input_file.resize((new_width, new_height), PIL.Image.BILINEAR)

    return input_file

def run_length_encode(data):
    # Given a set of bytes of data, return a list of bytes encoded with the following method
    # If a byte is not repeated, write the byte to the new list
    # If a byte is repeated up to 255 times, write two bytes:
    #   - one byte contains the value of the data byte with the most significant bit set to 1,
    #  - the other byte contains the number of times the byte was repeated
    # Continue until the original list has been fully processed

    output_data = []
    i = 0
    while i < len(data):
        # Get the current byte
        current_byte = data[i]

        # Get the number of times the current byte is repeated
        repeat_count = 1
        while i + repeat_count < len(data) and data[i + repeat_count] == current_byte and repeat_count < 255:
            repeat_count += 1

        # If the current byte is repeated more than once, write the byte with the most significant bit set
        if repeat_count > 1:
            output_data.append(current_byte | 0x80)
            output_data.append(repeat_count)
        else:
            output_data.append(current_byte)
        i += repeat_count
    return output_data

def get_used_colors(color_data):
    # Given a list of RGB triplets, count the number of unique triplets are in the list and return the count

    # Create a set to store the unique colors   
    unique_colors = set()

    # For each color in the list - note that each color consists of 3 sequential elements in the list
    for i in range(0, len(color_data), 3):
        # Get the red, green, and blue components of the color
        red = color_data[i]
        green = color_data[i + 1]
        blue = color_data[i + 2]

        # Add the color to the set
        unique_colors.add((red, green, blue))

    # If (0,0,0) is in the set and there are more than 64 colors, remove it
    if (0,0,0) in unique_colors and len(unique_colors) > 64:
        unique_colors.remove((0,0,0))

    # Return the number of unique colors
    return len(unique_colors)

# When quantizing an image to a palette, it puts the entire palette in the image.
#
# This function is a quick and dirty one - it takes the palletized image, bins
# the pixels to get the actual colors used in the palette, makes a optimal
# palette from that, and then requantizes the image with the optimal palette.
# The net result is an image where every color is the same as before, but 
# the palette contains no entries for colors that aren't used.
def shrink_palette(image):
    palette=image.getpalette()
    unique_colors = set()
    
    for j in range(image.height):
        for i in range(image.width):
            pixel = image.getpixel((i,j))
            unique_colors.add((pixel, palette[pixel*3], palette[pixel*3+1], palette[pixel*3+2]))
    
    final_palette = []
    for c in unique_colors:
        final_palette.append(c[1])
        final_palette.append(c[2])
        final_palette.append(c[3])
       
    temp_image = PIL.Image.new("P", [100, 100])
    temp_image.putpalette(final_palette)
    image = image.convert("RGB")
    image = image.quantize(len(final_palette), palette=temp_image)    
    
    return image
    
def main():
    # Read 4 command line parameters and store them in variables
    input_dir = sys.argv[1]
    output_dir = sys.argv[2]
    metadata_file = sys.argv[3]
    compression_type = sys.argv[4]
    use_transparency = sys.argv[5]

    # Get a list of all files in the input directory with the extension .pcx, .jpg, or .png
    input_files = [file for file in os.listdir(input_dir) if file.endswith(".pcx") or file.endswith(".jpg") or file.endswith(".png")]

    # Open the metadata file
    metadata = open(metadata_file, "r")

    # Read the CSV data into a list of lines
    metadata_lines = metadata.readlines()

    # Close the metadata file
    metadata.close()

    palette_base = PIL.Image.open(palette_base_image)
    
    # for each file in input_files
    for file in input_files:
        print(f"Processing {file}...")

        # Get the name of the file without the extension
        file_name = os.path.splitext(file)[0]

        # Use PIL to open the file
        image = PIL.Image.open(os.path.join(input_dir, file))

        # resize it if needed
        image = resize(image)

        # if the image has a palette or is grayscale, we'll still convert to RGB/RGBA depending on if
        # it has a alpha channel or not
        if image.mode == 'P' or image.mode == 'L':
            if 'transparency' in image.info:
                image = image.convert('RGBA')
            else:
                image = image.convert('RGB')


        # If the picture has an alpha channel, extract it for future use, then 
        # flatten the image, using white pixels where the transparency used to be
        alpha_channel = []
        if image.mode == 'RGBA':
            # Extract the alpha channel for now since we might use it
            alpha_channel = image.getchannel('A')
            temp_image = PIL.Image.new('RGBA', image.size, (255, 255,255, 255))
            temp_image.paste(image, mask=alpha_channel)
            # Convert the image to RGB
            image = temp_image.convert('RGB')
            # After flattening the image using the alpha mask, mark the alpha channel either 
            # 0 for transparent or 1 for opaque to make it easier on the other side
            for j in range(alpha_channel.height):
                for i in range(alpha_channel.width):
                    if alpha_channel.getpixel((i,j)) != 0:
                        alpha_channel.putpixel((i, j), 1)

        # Get the palette from the image (or 'None' if it's RGB)
        palette = image.getpalette()

        # At this point, the image should have a palette of None, since everything
        # has been converted to RGB.  If not, this will all still work, but images
        # with palettes of more than 64 colors will be skipped later.
        # Quantize to 64 colors
        if palette == None:
            image = image.quantize(colors=64, palette=palette_base, dither=PIL.Image.Dither.NONE)
            image = shrink_palette(image)
            palette = image.getpalette()

        # Now the image should have a palette.  Find how many colors it has.
        # If there's more than 64 of them, skip the image
        num_colors = get_used_colors(palette)
        print(f" This image has {num_colors} colors")
        if num_colors > 64:
            print(f"WARNING: {file} still has more than 64 colors, skipping...")
            continue

        # This step is tricky.  If we're using transparency, we need to count the pixels in the 
        # image and determine if there are any pixels in the image with value (255, 255, 255)
        # that aren't part of the transparency mask
        # If there are, then do nothing; we can use the image as is
        # On the other hand, if there aren't:
        #  - Get the index of (255, 255, 255)
        #  - Remove it and shift all values past it up one
        #  - Scan the whole image, and for all values that were shifted, change the value of the
        #    pixel to (pixel value - 1)
        #  - Set num_colors to num_colors - 1
        #
        # Note - in the alpha mask, 0 is transparent and 255 is opaque
        if use_transparency == "1":
            white_count = 0
            index = -1
            # get the index of (255, 255, 255) in palette, or -1 if it's not there
            for i in range(0, len(palette), 3):
                if palette[i] == 255 and palette[i+1] == 255 and palette[i+2] == 255:
                    index = int(i/3)
                    print(f"White found in palette position {index}")
            for j in range(alpha_channel.height):
                for i in range(alpha_channel.width):
                    if alpha_channel.getpixel((i, j)) == 1 and image.getpixel((i, j)) == index:
                        white_count = white_count + 1
            print(f"There are {white_count} non-transparent white pixels")
            if white_count == 0:
                if index == -1:
                    print("Image probably isn't transparent - skipping")
                    use_transparency = '0'
                else:
                    print(f"No other white pixels found - shifting palette...")
                    print(f"White palette entry found at {index}")
                    # Take all values after the index and shift them up one
                    for i in range(index * 3, len(palette)-3, 3):
                        palette[i] = palette[i+3]
                        palette[i+1] = palette[i+4]
                        palette[i+2] = palette[i+5]
                    palette[len(palette)-3] = 0
                    palette[len(palette)-2] = 0
                    palette[len(palette)-1] = 0
                    # Scan the whole image and change all values that were shifted
                    for j in range(image.height):
                        for i in range(image.width):
                            if image.getpixel((i, j)) > index:
                                image.putpixel((i, j), image.getpixel((i, j)) - 1)
                    image.putpalette(palette)
                    num_colors = num_colors - 1
                    palette[num_colors*3] = 0
                    palette[num_colors*3+1] = 0
                    palette[num_colors*3+1] = 0

        # Get the width and height of the image
        width, height = image.size

        # Get the pixel data from the image
        pixel_data = list(image.getdata())
                    
        category_id = 0
        displayed_name = "Default Image"

        # Find the line in the metadata file where the first field matches the input file name
        for line in metadata_lines:
            # Split the line into a list of fields
            fields = line.split(",")
            # If the first field matches the input file name
            if fields[0] == file:
                # Get the category ID from the second field
                displayed_name = fields[1]
                # Get the displayed name from the third field
                category_id = int(fields[2])
                # Stop searching
                break

        # Create a new file with the same name as the input file, but with the extension .pic
        output_file = open(os.path.join(output_dir, file_name + '.pic'), "wb")

        # Write the bytes 'DP' to the file
        output_file.write(b"DPTM")
        # Write the width and height of the image to the file
        output_file.write(width.to_bytes(2, byteorder="little"))
        output_file.write(height.to_bytes(2, byteorder="little"))
        # Write the category ID (as one byte) to the file
        output_file.write(int(category_id).to_bytes(1, byteorder="little"))
        # If the length of the displayed name is greater than 32, truncate it
        if len(displayed_name) > 32:
            displayed_name = displayed_name[:32]
        # Otherwise, if the length of the displayed name is less than 32, pad it with zeroes
        elif len(displayed_name) < 32:
            displayed_name = displayed_name + "\x00" * (32 - len(displayed_name))
        # Write the displayed name to the file
        output_file.write(bytes(displayed_name, "ascii"))

        rle_data = run_length_encode(pixel_data)

        write_rle = False
        if compression_type == "1":
            write_rle = True
        elif compression_type == "2":
            if len(rle_data) < len(pixel_data):
                write_rle = True


        # Write the number of colors in the palette to the file
        output_file.write(num_colors.to_bytes(1, byteorder="little"))
     
        if write_rle == True:
            output_file.write(b"\x01")
        else:
            output_file.write(b"\x00")

        palette_data = create_ascii_palette(palette, num_colors)        
        output_file.write(bytes(palette_data))

        if use_transparency == "1":
            # Count the number of zeroes in alpha_channel
            num_playable_squares = 0
            for j in range(alpha_channel.height):
                for i in range(alpha_channel.width):
                    if alpha_channel.getpixel((i, j)) != 0:
                        num_playable_squares = num_playable_squares + 1
            # Write the transparency flag, non-transparent_squares and 22 0 bytes to the file
            output_file.write(b"\x01")
            output_file.write(num_playable_squares.to_bytes(2, byteorder="little"))
            output_file.write(b"\x00" * 18)
        else:
            # Write 23 0 bytes to the file
            output_file.write(b"\x00" * 21)

        if write_rle == False:
            output_file.write(bytes(pixel_data))
        else:
            output_file.write(bytes(rle_data))

        if use_transparency == "1":
            alpha_pixels = list(alpha_channel.getdata())
            if write_rle == False:
                output_file.write(bytes(alpha_pixels))
            else:
                alpha_rle_pixels = run_length_encode(alpha_pixels)
                output_file.write(bytes(alpha_rle_pixels))

        # Close the file
        output_file.close()

        if debug_output == True:
            image.save(os.path.join(output_dir, file_name + '_converted' + '.png'), "PNG")

if __name__ == "__main__":
    main()

