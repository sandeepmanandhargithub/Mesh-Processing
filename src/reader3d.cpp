#include "reader3d.h"
#include <QDebug>

string UpperCase( const string& s )
{
    // Upper case string
    string us;

    // For every character in the string
    string::const_iterator it = s.begin();
    while( it != s.end() )
    {
        // Convert character to upper case
        us += toupper( *it );

        // Next character
        ++it;
    }

    // Return upper case string
    return us;
}
void dummy(){
    qDebug()<<"af";
}

bool ReadIv(object3D& mesh, const string& file_name) {

    //cout<<"Reading IV/VRML"<<endl;
    ifstream file; // File to read
    string line, new_line; // Lines to process
    string word, previous_word; // Words to process
    string::iterator it; // Iterator for lines
    size_t pos; // String position
    int nlbrack(0), nrbrack(0); // Left/Right delimiter counter
    int level(0); // Node level
    int ixyz(0); // Coordinate index
    Vector3d vec3d; // Temp vector 3D (vertex, color, normal)
    Vector3i vec3i; // Temp vector 3I (face)
    Vector2d vec2d; // Temp vector 2D (texture coordinate)
    //AttributeBinding normal_binding(PER_VERTEX_BINDING); // Temp normal binding
    //AttributeBinding texture_binding(PER_VERTEX_BINDING); // Temp texture binding
    vector<string> node(10); // nodes - 10 levels by default

    // Open file
    file.open( file_name.c_str() );

    // Test if file is open
    if( file.is_open() == false )
    {
        cout<<"No file found"<<endl;
        return false;
    }

    //--
    // Read file header
    //--

    // Get first line of the file
    getline( file, line );

    // Try to find #Inventor in the line
    pos = line.find("#Inventor");
    if( pos > line.length() )
    {
        // Else try to find #VRML V1.0 in the line
        pos = line.find("#VRML V1.0");
        if( pos > line.length() )
        {
            cout<<"pas VRML"<<endl;
            return false;
        }
    }

    // Verify file is ascii format
    pos = line.find("ascii");
    if( pos > line.length() )
    {
        return false;
    }

    // Initialize the mesh
    mesh.ClearAll();

    //--
    // Read the file until the end reading line by line
    //--
    while( getline( file, line ) )
    {
        // Replace all comas by space
        replace( line.begin(), line.end(), ',', ' ' );

        // Initialize new_line
        new_line.erase( new_line.begin(), new_line.end() );

        //--
        // Buffer brackets and braces by space and copy in new_line
        //--
        it = line.begin();
        while( it != line.end() )
        {

            if( ((*it)=='{') || ((*it)=='}') || ((*it)=='[') || ((*it)==']') || ((*it)=='#') )
            {
                new_line += ' ';
                new_line += *it++;
                new_line += ' ';
            }
            else
            {
                new_line += *it++;
            }
        }

        //--
        // Analyse each word in the line
        //--
        istringstream word_stream( new_line );
        while( word_stream >> word )
        {
            //--
            // Left bracket or brace
            //--
            if( (word == "{") || (word == "[") )
            {
                // Increment left delimiter number
                nlbrack++;
                // Get level number
                level = nlbrack - nrbrack;
                // Save level name
                if( level > (int)node.size() )
                {
                    node.push_back( UpperCase(previous_word) );
                }
                else
                {
                    node[level] = UpperCase(previous_word);
                }
                // Initialize coordinate index
                ixyz = 0;
            }

            //--
            // Right bracket or brace
            //--
            else if( (word == "}") || (word == "]") )
            {
                // Increment right deliminter number
                nrbrack++;
                // Get level number
                level = nlbrack - nrbrack;
                // Sanity test
                if( level < 0 )
                {
                    mesh.ClearAll();
                    return false;
                }
            }

            //--
            // Comment
            //--
            else if( word == "#" )
            {
                // Save current word
                previous_word = word;
                // Next line
                break;
            }

            //--
            // COORDINATE3
            //--
            else if( node[level] == "COORDINATE3" )
            {
            }

            //--
            // INDEXEDFACESET
            //--
            else if( node[level] == "INDEXEDFACESET" )
            {
            }

            //--
            // MATERIAL
            //--
            else if( node[level] == "MATERIAL" )
            {
            }

            //--
            // MATERIALBINDING
            //--
            else if( node[level] == "MATERIALBINDING" )
            {
                if( previous_word == "value" )
                {
                    //mesh.ColorBinding() = StringToBinding( word );
                }
            }

            //--
            // NORMALBINDING
            //--
            else if( node[level] == "NORMALBINDING" )
            {
                if( previous_word == "value" )
                {
                    //normal_binding = StringToBinding( word );
                }
            }

            //--
            // TEXTURECOORDINATEBINDING
            //--
            else if( node[level] == "TEXTURECOORDINATEBINDING" )
            {
                if( previous_word == "value" )
                {
                    //texture_binding = StringToBinding( word );
                }
            }

            //--
            // POINT
            //--
            else if( node[level] == "POINT" )
            {
                if( node[level-1] == "COORDINATE3" )
                {
                    // Get current value
                    vec3d[ixyz] = atof( word.c_str() );
                    // Complete coordinate ?
                    if( ixyz == 2 )
                    {
                        mesh.AddVertex( vec3d );
                        ixyz = 0;
                    }
                    else
                    {
                        ixyz++;
                    }
                }
                else if( node[level-1] == "TEXTURECOORDINATE2" )
                {
                    // Get current value
                    vec2d[ixyz] = atof( word.c_str() );
                    // Complete coordinate ?
                    if( ixyz == 1 )
                    {
                        //mesh.AddTexture( vec2d );
                        ixyz = 0;
                    }
                    else
                    {
                        ixyz++;
                    }
                }
            }

            //--
            // DIFFUSECOLOR
            //--
            else if( node[level] == "DIFFUSECOLOR" )
            {
                if( node[level-1] == "MATERIAL" )
                {
                    // Get current value
                    vec3d[ixyz] = atof( word.c_str() );
                    // Complete coordinate ?
                    if( ixyz == 2 )
                    {
                        //mesh.AddColor( vec3d );
                        ixyz = 0;
                    }
                    else
                    {
                        // Next coordinate
                        ixyz++;
                    }
                }
            }

            //--
            // VECTOR
            //--
            else if( node[level] == "VECTOR" )
            {
                if( node[level-1] == "NORMAL" )
                {
                    // Get current value
                    vec3d[ixyz] = atof( word.c_str() );
                    // Complete coordinate ?
                    if( ixyz == 2 )
                    {
                        mesh.AddVertexNormal( vec3d );
                        ixyz = 0;
                    }
                    else
                    {
                        // Next coordinate
                        ixyz++;
                    }
                }
            }

            //--
            // TEXTURE2
            //--
            else if( node[level] == "TEXTURE2" )
            {
                if( previous_word == "filename" )
                {
                    // Get texture filename
                    //mesh.TextureName() = word;
                }
            }

            //--
            // COORDINDEX
            //--
            else if( node[level] == "COORDINDEX" )
            {
                if( node[level-1] == "INDEXEDFACESET" )
                {
                    // -1 value
                    if( ixyz == 3 )
                    {
                        // Next face
                        ixyz = 0;
                        continue;
                    }
                    // Get value
                    vec3i[ixyz] = atoi( word.c_str() );
                    // Complete coordinate ?
                    if( ixyz == 2 )
                    {
                        mesh.AddFace( vec3i );
                    }
                    // Next coordinate
                    ixyz++;
                }
            }

            //--
            // MATERIALINDEX
            //--
            else if( node[level] == "MATERIALINDEX" )
            {
                if( node[level-1] == "INDEXEDFACESET" )
                {
                }
            }

            //--
            // TEXTURECOORDINDEX
            //--
            else if( node[level] == "TEXTURECOORDINDEX" )
            {
                if( node[level-1] == "INDEXEDFACESET" )
                {
                }
            }

            //--
            // NORMALINDEX
            //--
            else if( node[level] == "NORMALINDEX" )
            {
                if( node[level-1] == "INDEXEDFACESET" )
                {
                }
            }

            // Save current word
            previous_word = word;
        }
    }

    // Check vertex normals
   // if( (normal_binding!=PER_VERTEX_BINDING) || (mesh.VertexNormalNumber()!=mesh.VertexNumber()) )
  //  {
   //     mesh.ClearVertexNormals();
   // }
    mesh.ComputeFaceNormals();
    mesh.ComputeVertexNormals();

    // Normalize normals
//    if( mesh.VertexNormalNumber() != 0 )
//    {
//        for( int i=0; i<mesh.VertexNormalNumber(); i++ )
//        {
//            mesh.VertexNormal(i).normalized();
//        }
//    }

    // Check texture coordinates
//    if( (texture_binding!=PER_VERTEX_BINDING) || (mesh.TextureNumber()!=mesh.VertexNumber()) )
//    {
//        mesh.ClearTextures();
//        mesh.ClearTextureName();
//    }

    // Check colors
//    if( (mesh.ColorBinding()==PER_FACE_BINDING) && (mesh.ColorNumber()!=mesh.FaceNumber()) )
//    {
//        mesh.ClearColors();
//        mesh.ColorBinding() = UNKNOWN_BINDING;
//    }
//    else if( (mesh.ColorBinding()==PER_VERTEX_BINDING) && (mesh.ColorNumber()!=mesh.VertexNumber()) )
//    {
//        mesh.ClearColors();
//        mesh.ColorBinding() = UNKNOWN_BINDING;
//    }

    file.close();

    return true;
}
