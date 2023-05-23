# FF: FILE FORMATS

## SYNOPSIS

Details regarding the purpose of the currently existing file formats.

## FOREWORD

One of the first concerns when thinking of making a library of game assets was the cost in terms of storage and bandwidth use: and since funds are lacking, throwing money at the problem is not an option.

So the library needs to be small -- in terms of filesize rather than content. Thus began a long research process into data compression, way longer than anyone would've liked, but I will not complain about the results.

There are two mainline tricks, one of which is essentially just a zlib wrapper that does very little on it's own. The other is rather a *collection* of smaller tricks that serve the same overall purpose: increasing redundancy of the data as much as possible.

These you could think of as the two layers of file compression in use. First off, assets are bundled together by some "tileset" criteria; if they'll likely be used together, then they'll be writ together. The files needed to load these assets are generated from Blender data and passed through one or another form of quantization, depending on the nature of the data being held. Then, all of the quantized files are packed together in a single archive.

Nearly all forms of quantization utilize `frac` (see: [kvrnel/Bytes](https://github.com/Liebranca/bitter/blob/e701daac9df4a300c0539c608c1881f6b4c4e98f/kvrnel/Bytes.hpp#L389)) to avoid floating point numbers entirely; we do trade a lot of precision for our smaller filesizes. As such, retro and minimalist modeling and texturing are bound to work better.

Following are brief overviews of each file format.

## THE DARK ARCHIVE FORMAT

As previously stated, DAF is merely a zlib wrapper; it's a general purpose container holding both the quantized data and a file table for ease of extraction and modification of archives, all stored sequentially and zipped into a single block. 

By itself, this zipping does little: similar, highly redundant blocks being packed together into an archive is what allows the compression to achieve anything significant; as such, "tileset" criteria comes into play much more strongly than anything else, meaning: assets using similar patterns in their data, be it color, shape or deforms, *must* be packed together to get the better compression.

## THE JOJ FORMAT

Not an acronym, merely an ancient meme I find amusing. The JOJ is designed to pack multiple images into a single file (be it material textures or spritesheets), and saves space by breaking them into tiles that are writ together as an atlas, ignoring blank spaces in all images.

JOJ utilizes special encodings for different types of image data. The most basic is for color data with an alpha channel: the RGBA pixels are converted into YUV+alpha, with each channel subsequently frac'd.

The second is for ORME or *shading* maps, which hold __O__cllusion, __R__oughness, __M__etalness and __E__mission maps packed into a single RGBA image. This one is also fairly simple, utilizing only frac'ing of each grayscale image to compose the final result.

Third is normal+curvature, or NC maps. These are slightly more complex, as the normal part is encoded as spherical coordinates (see: [kvrnel/Seph](https://github.com/Liebranca/bitter/blob/e701daac9df4a300c0539c608c1881f6b4c4e98f/kvrnel/src/Seph.cpp#L24)) rather than color. The curvature part uses a single channel and is frac'd as the others grayscale maps.

What the JOJ does, in essence, is create a separate atlas for each type of texture map requested, that is: one atlas for YUV+A, another for ORME, and a third for NC. If only saving one or two types of atlases, the unused ones are not created.

## THE CRK FORMAT

A corrupted way to abbreviate corrupted, CRK takes care of storing meshes. It's workings are much less involved than JOJ, and hopefully easier to grasp.

In essence, CRK will make the very bold assumption that all and every piece of 3D geometry is made up of pixel-clamped UVs and points in space within a small spherical range from the scene origin; that is, UV islands must be packed with care and vertices cannot be farther away from the origin than a fixed distance of four meters. 

Following this assumption, UVs are frac'd normally and every point is stored as a set of spherical coordinates. The encoding is identical to JOJ normals for the vertex normal, tangent and bitangent vectors, and *nearly* the same for vertex coordinates themselves: the radius of the sphere is added on top of slightly higher precision for the azimuth and inclination angles.

This does mean that whenever larger space is required, assets must be broken down into separate chunks: but this is what one would anyway do in such cases for the sake of modularity, so generally it shouldn't even matter.

CRK meshes handle all types of animation (skeletal or otherwise) by way of memoization: instead of skinning a mesh at runtime, the deforms are directly baked to the file pose by pose.

The obvious con is higher memory usage based on vertex count times number of frames of animation; the advantage is that no deforms need to be calculated, and thus drawing both deforming and static meshes has virtually the same cost. 

Additionally, and as tested on a cheap integrated graphics chip from the early 2010s, SIN vertex shaders (see: [CRK_Angular](https://github.com/Liebranca/sin/blob/main/mesh/src/CRK_Angular.sg)) can decode CRK meshes with minimal performance impact to ease the memory cost, and that will still be faster than GPU skinning.

## THE ANS FORMAT

An arspeak shortening of 'animations'. Originally, this was the format we utilized to encode armatures for skinned meshes, as well as the animations themselves. Over time, armatures were discarded in favor of the pure CRK system previously discussed, and most of the uses of ANS made redundant.

Currently, ANS files are only used for animation metadata. Additional uses might be packed into them as time progresses.
