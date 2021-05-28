/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== logo.svg ==================
static const unsigned char temp_binary_data_0[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
"<svg xmlns=\"http://www.w3.org/2000/svg\" style=\"isolation:isolate\" viewBox=\"0 0 32 32\" width=\"32pt\" height=\"32pt\">\n"
"    <path d=\" M 25.812 9.469 C 25.455 9.243 25.08 9.046 24.689 8.88 C 23.738 8.479 22.716 8.272 21.684 8.273 C 20.652 8.272 19.63 8.479 18.679 8.88 C 17.761 9.269 16.926 9.832 16.222 10.538 C 15.516 11.242 14.953 12.077 14.564 12.993 C 14.163 13.945"
" 13.956 14.968 13.957 16 L 13.957 16 C 13.956 17.032 14.163 18.054 14.564 19.005 C 14.953 19.923 15.516 20.758 16.222 21.462 C 16.926 22.168 17.761 22.731 18.679 23.12 C 19.63 23.521 20.652 23.728 21.684 23.727 C 22.716 23.728 23.738 23.521 24.689 23"
".12 C 25.607 22.731 26.441 22.168 27.146 21.462 C 27.852 20.758 28.415 19.923 28.804 19.005 C 29.205 18.054 29.412 17.032 29.411 16 C 29.412 14.968 29.205 13.946 28.804 12.995 C 28.638 12.604 28.441 12.229 28.215 11.872 L 22.885 17.202 L 20.482 14.79"
"8 L 25.812 9.469 Z  M 11.368 16 C 11.368 10.306 15.99 5.684 21.684 5.684 C 27.378 5.684 32 10.306 32 16 C 32 21.694 27.378 26.316 21.684 26.316 C 15.99 26.316 11.368 21.694 11.368 16 L 11.368 16 Z \"\n"
"          fill-rule=\"evenodd\" fill=\"rgb(255,255,255)\"/>\n"
"    <path d=\" M 2.427 9.042 L 0 9.042 L 0 16.485 L 7.444 16.485 L 7.444 9.042 L 5.016 9.042 L 5.016 9.042 L 2.427 9.042 L 2.427 9.042 L 2.427 9.042 Z  M 2.427 8.556 L 2.427 5.684 L 5.016 5.684 L 5.016 8.556 L 2.427 8.556 L 2.427 8.556 Z  M 2.427 16."
"971 L 2.427 26.316 L 5.016 26.316 L 5.016 16.971 L 2.427 16.971 L 2.427 16.971 L 2.427 16.971 Z \"\n"
"          fill-rule=\"evenodd\" fill=\"rgb(255,255,255)\"/>\n"
"</svg>";

const char* logo_svg = (const char*) temp_binary_data_0;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x78dee5d0:  numBytes = 1582; return logo_svg;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "logo_svg"
};

const char* originalFilenames[] =
{
    "logo.svg"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
    {
        if (namedResourceList[i] == resourceNameUTF8)
            return originalFilenames[i];
    }

    return nullptr;
}

}
