
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <climits>
#include <cstdio>

#include "program.h"


// charge un fichier texte.
static
std::string read( const char *filename )
{
    std::stringbuf source;
    std::ifstream in(filename);
    if(in.good() == false)
        printf("[error] loading program '%s'...\n", filename);
    else
        printf("loading program '%s'...\n", filename);
    
    in.get(source, 0);        // lire tout le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
    return source.str();
}

// insere les definitions apres la ligne contenant #version
static 
std::string prepare_source( std::string file, const std::string& definitions )
{
    if(file.empty()) 
        return std::string();
    
    // un peu de gymnastique, #version doit rester sur la premiere ligne, meme si on insere des #define dans le source
    std::string source;
    
    // recupere la ligne #version
    std::string version;
    size_t b= file.find("#version");
    if(b != std::string::npos)
    {
        size_t e = file.find( '\n', b );
        if(e != std::string::npos)
        {
            version= file.substr(0, e +1);
            file.erase(0, e +1);
            
            if(file.find("#version") != std::string::npos)
            {
                printf("[error] found several #version directives. failed.\n");
                return std::string();
            }
        }
    }
    else
    {
        printf("[error] no #version directive found. failed.\n");
        return std::string();
    }
    
    // reconstruit le source complet
    if(definitions.empty() == false)
    {
        source.append(version);                         // insere la version
        source.append(definitions).append("\n");        // insere les definitions
        source.append(file);                            // insere le source
    }
    else
    {
        source.append(version);                         // re-insere la version (supprimee de file)
        source.assign(file);                            // insere le source
    }
    //printf("%s\n", source.c_str());
    return source;
}

static
GLuint compile_shader( const GLuint program, const GLenum shader_type, const std::string& source )
{
    if(source.size() == 0) 
        return 0;
    
    GLuint shader= glCreateShader(shader_type);
    glAttachShader(program, shader);
    
    const char *sources= source.c_str();
    glShaderSource(shader, 1, &sources, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
		GLint log_size;
		GLsizei size;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
        std::string outLogs;
		outLogs.resize(static_cast<size_t>(log_size) + 1);
		glGetShaderInfoLog(shader, log_size, &size, &outLogs[0]);
        printf("Shader error:\n%s", outLogs.c_str());
    }
    return (status == GL_TRUE) ? shader : 0;
}


int reload_program( GLuint program, const char *filename, const char *definitions )
{
    if(program == 0) 
        return -1;

    // supprime les shaders attaches au program
    int shaders_max= 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaders_max);
    if(shaders_max > 0)
    {
        std::vector<GLuint> shaders(shaders_max, 0);
        glGetAttachedShaders(program, shaders_max, NULL, &shaders.front());
        for (int i = 0; i < shaders_max; i++)
        {
            glDetachShader(program, shaders[i]);
            glDeleteShader(shaders[i]);
        }
    }

#ifdef GL_VERSION_4_3
    glObjectLabel(GL_PROGRAM, program, -1, filename);
#endif
    
    // prepare les sources
    std::string common_source= read(filename);
    std::string vertex_source= prepare_source(common_source, std::string(definitions).append("#define VERTEX_SHADER\n"));
    std::string fragment_source= prepare_source(common_source, std::string(definitions).append("#define FRAGMENT_SHADER\n"));

    // cree et compile un vertex shader et un fragment shader
    GLuint vertex_shader= compile_shader(program, GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader= compile_shader(program, GL_FRAGMENT_SHADER, fragment_source);
    // linke les shaders
    glLinkProgram(program);
    
    // verifie les erreurs
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) 
    {
        if(vertex_shader == 0)
            printf("[error] compiling vertex shader...\n%s\n", definitions);
        if(fragment_shader == 0)
            printf("[error] compiling fragment shader...\n%s\n", definitions);
        printf("[error] linking program %u '%s'...\n", program, filename);
		GLint log_size;
		GLsizei size;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
        std::string OutLogs;
		OutLogs.resize(static_cast<size_t>(log_size) + 1);
		glGetProgramInfoLog(program, log_size, &size, &OutLogs[0]);
        printf("%s\n", OutLogs.c_str());
        return -1;
    }
    
    // pour etre coherent avec les autres fonctions de creation, active l'objet gl qui vient d'etre cree.
    glUseProgram(program);
    return 0;
}

GLuint read_program( const char *filename, const char *definitions )
{
    GLuint program= glCreateProgram();
    reload_program(program, filename, definitions);
    return program;
}

int release_program( const GLuint program )
{
    if(program == 0)
        return -1;
    
    // recupere les shaders
    int shaders_max= 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaders_max);
    
    std::vector<GLuint> shaders(shaders_max, 0);
    glGetAttachedShaders(program, shaders_max, NULL, &shaders.front());
    for(int i= 0; i < shaders_max; i++)
    {
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    
    glDeleteProgram(program);
    return 0;
}


// formatage des erreurs de compilation des shaders

static
void print_line( std::string& errors, const char *source, const int begin_id, const int line_id )
{
    int line= 0;
    char last= '\n';
    for(unsigned int i= 0; source[i] != 0; i++)
    {
        if(line > line_id)
            break;

        if(last == '\n')
        {
            line++;
            if(line >= begin_id && line <= line_id)
            {
                errors.append("  ");
                errors.push_back('0' + (line / 1000) % 10);
                errors.push_back('0' + (line / 100) % 10);
                errors.push_back('0' + (line / 10) % 10);
                errors.push_back('0' + (line / 1) % 10);
                errors.append("  ");
            }
        }
        
        if(line >= begin_id && line <= line_id)
        {
            if(source[i] == '\t')
                errors.append("    ");
            else
                errors.push_back(source[i]);
        }
        last= source[i];
    }
}

static
int print_errors( std::string& errors, const char *log, const char *source )
{
    printf("[error log]\n%s\n", log);
    
    int first_error= INT_MAX;
    int last_string= -1;
    int last_line= -1;
    for(int i= 0; log[i] != 0; i++)
    {
        // recupere la ligne assiciee a l'erreur
        int string_id= 0, line_id= 0, position= 0;
        if(sscanf(&log[i], "%d ( %d ) : %n", &string_id, &line_id, &position) == 2        // nvidia syntax
        || sscanf(&log[i], "%d : %d (%*d) : %n", &string_id, &line_id, &position) == 2  // mesa syntax
        || sscanf(&log[i], "ERROR : %d : %d : %n", &string_id, &line_id, &position) == 2  // ati syntax
        || sscanf(&log[i], "WARNING : %d : %d : %n", &string_id, &line_id, &position) == 2)  // ati syntax
        {
            if(string_id != last_string || line_id != last_line)
            {
                // conserve la premiere erreur
                first_error= std::min(first_error, line_id);
                
                // extrait la ligne du source...
                errors.append("\n");
                print_line(errors, source, last_line +1, line_id);
                errors.append("\n");
            }
        }
        // et affiche l'erreur associee...
        for(i+= position; log[i] != 0; i++)
        {
            errors.push_back(log[i]);
            if(log[i] == '\n')
                break;
        }
        
        last_string= string_id;
        last_line= line_id;
    }
    errors.append("\n");
    print_line(errors, source, last_line +1, 1000);
    errors.append("\n");
    
    return first_error;
}

int program_format_errors( const GLuint program, std::string& errors )
{
    errors.clear();
    
    if(program == 0)
    {
        errors.append("[error] no program...\n");
        return -1;
    }
    
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) 
        return 0;
    
    int first_error= INT_MAX;
    
    // recupere les shaders
    int shaders_max= 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaders_max);
    
    std::vector<GLuint> shaders(shaders_max, 0);
    glGetAttachedShaders(program, shaders_max, NULL, &shaders.front());
    for(int i= 0; i < shaders_max; i++)
    {
        GLint value;
        glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &value);
        if(value == GL_FALSE)
        {
            // recupere les erreurs de compilation des shaders
            glGetShaderiv(shaders[i], GL_INFO_LOG_LENGTH, &value);
            std::vector<char>log(value+1, 0);
            glGetShaderInfoLog(shaders[i], (GLsizei) log.size(), NULL, &log.front());
            
            // recupere le source
            glGetShaderiv(shaders[i], GL_SHADER_SOURCE_LENGTH, &value);
            std::vector<char> source(value+1, 0);
            glGetShaderSource(shaders[i], source.size(), NULL, &source.front());
            
            glGetShaderiv(shaders[i], GL_SHADER_TYPE, &value);
            errors.append("[error] compiling ");
            if(value == GL_VERTEX_SHADER)
                errors.append("vertex shader...\n");
            else if(value == GL_FRAGMENT_SHADER)
                errors.append("fragment shader...\n");
            else
                errors.append("shader...\n");
            
            // formatte les erreurs
            int last_error= print_errors(errors, &log.front(), &source.front());
            first_error= std::min(first_error, last_error);
        }
    }
    
    // recupere les erreurs de link du program
    {
        GLint value= 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &value);
        
        std::vector<char>log(value+1, 0);
        glGetProgramInfoLog(program, (GLsizei) log.size(), NULL, &log.front());
        
        errors.append("[error] linking program...\n").append(log.begin(), log.end());
    }
    
    return first_error;
}

int program_print_errors( const GLuint program )
{
    std::string errors;
    int code= program_format_errors(program, errors);
    if(errors.size() > 0) 
        printf("%s\n", errors.c_str());
    return code;
}

