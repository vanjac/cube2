#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "cube.h"
#include "world.h"

#ifndef STANDALONE

#include "glexts.h"
#include "octa.h"
#include "lightmap.h"
#include "bih.h"
#include "texture.h"
#include "model.h"
#include "varray.h"

extern dynent *player;
extern physent *camera1;                // special ent that acts as camera, same object as player1 in FPS mode

extern int worldscale, worldsize;
extern int mapversion;
extern char *maptitle;
extern vector<ushort> texmru;
extern int xtraverts, xtravertsva;
extern const ivec cubecoords[8];
extern const ivec facecoords[6][4];
extern const uchar fv[6][4];
extern const uchar fvmasks[64];
extern const uchar faceedgesidx[6][4];
extern bool inbetweenframes, renderedframe;

extern SDL_Surface *screen;
extern int zpass, glowpass;

extern vector<int> entgroup;

// rendertext
struct font
{
    struct charinfo
    {
        short x, y, w, h, offsetx, offsety, advance, tex;
    };

    char *name;
    vector<Texture *> texs;
    vector<charinfo> chars;
    int charoffset, defaultw, defaulth, scale;

    font() : name(NULL) {}
    ~font() { DELETEA(name); }
};

#define FONTH (curfont->scale)
#define FONTW (FONTH/2)
#define MINRESW 640
#define MINRESH 480

extern font *curfont;

// texture
extern int hwtexsize, hwcubetexsize, hwmaxaniso, maxtexsize;

extern Texture *textureload(const char *name, int clamp = 0, bool mipit = true, bool msg = true);
extern int texalign(void *data, int w, int bpp);
extern void cleanuptexture(Texture *t);
extern void loadalphamask(Texture *t);
extern void loadlayermasks();
extern Texture *cubemapload(const char *name, bool mipit = true, bool msg = true, bool transient = false);
extern void drawcubemap(int size, const vec &o, float yaw, float pitch, const cubemapside &side);
extern void loadshaders();
extern void setuptexparameters(int tnum, void *pixels, int clamp, int filter, GLenum format = GL_RGB, GLenum target = GL_TEXTURE_2D);
extern void createtexture(int tnum, int w, int h, void *pixels, int clamp, int filter, GLenum component = GL_RGB, GLenum target = GL_TEXTURE_2D, int pw = 0, int ph = 0, int pitch = 0, bool resize = true, GLenum format = GL_FALSE);
extern void blurtexture(int n, int bpp, int w, int h, uchar *dst, const uchar *src, int margin = 0);
extern void blurnormals(int n, int w, int h, bvec *dst, const bvec *src, int margin = 0);
extern void renderpostfx();
extern void initenvmaps();
extern void genenvmaps();
extern ushort closestenvmap(const vec &o);
extern ushort closestenvmap(int orient, int x, int y, int z, int size);
extern GLuint lookupenvmap(ushort emid);
extern GLuint lookupenvmap(Slot &slot);
extern bool reloadtexture(Texture &tex);
extern bool reloadtexture(const char *name);
extern void setuptexcompress();
extern void clearslots();
extern void compacteditvslots();
extern void compactmruvslots();
extern void compactvslots(cube *c, int n = 8);
extern void compactvslot(int &index);
extern int compactvslots();

// shadowmap

extern int shadowmap, shadowmapcasters;
extern bool shadowmapping;

extern bool isshadowmapcaster(const vec &o, float rad);
extern bool addshadowmapcaster(const vec &o, float xyrad, float zrad);
extern bool isshadowmapreceiver(vtxarray *va);
extern void rendershadowmap();
extern void pushshadowmap();
extern void popshadowmap();
extern void rendershadowmapreceivers();
extern void guessshadowdir();

// pvs
extern void clearpvs();
extern bool pvsoccluded(const ivec &bbmin, const ivec &bbmax);
extern bool pvsoccludedsphere(const vec &center, float radius);
extern bool waterpvsoccluded(int height);
extern void setviewcell(const vec &p);
extern void savepvs(stream *f);
extern void loadpvs(stream *f, int numpvs);
extern int getnumviewcells();

static inline bool pvsoccluded(const ivec &bborigin, int size)
{
    return pvsoccluded(bborigin, ivec(bborigin).add(size));
}

// rendergl
extern bool hasVBO, hasDRE, hasMDA, hasOQ, hasTR, hasFBO, hasDS, hasTF, hasBE, hasBC, hasCM, hasNP2, hasTC, hasS3TC, hasFXT1, hasMT, hasAF, hasGLSL, hasNVFB, hasDT, hasPBO, hasFBB, hasUBO, hasMBR;
extern int hasstencil;
extern int glversion, glslversion;

extern float curfov, fovy, aspect, forceaspect;
extern bool envmapping, minimapping, renderedgame, modelpreviewing;
extern const glmatrixf viewmatrix;
extern glmatrixf mvmatrix, projmatrix, mvpmatrix, invmvmatrix, invmvpmatrix, fogmatrix, invfogmatrix, envmatrix;
extern bvec fogcolor;

extern void gl_checkextensions();
extern void gl_init(int w, int h, int bpp, int depth, int fsaa);
extern void cleangl();
extern void rendergame(bool mainpass = false);
extern void invalidatepostfx();
extern void gl_drawframe(int w, int h);
extern void gl_drawmainmenu(int w, int h);
extern void drawminimap();
extern void drawtextures();
extern void enablepolygonoffset(GLenum type);
extern void disablepolygonoffset(GLenum type);
extern void calcspherescissor(const vec &center, float size, float &sx1, float &sy1, float &sx2, float &sy2);
extern int pushscissor(float sx1, float sy1, float sx2, float sy2);
extern void popscissor();
extern void recomputecamera();
extern void findorientation();
extern void writecrosshairs(stream *f);

namespace modelpreview
{
    extern void start(bool background = true);
    extern void end();
}

// renderextras
extern void render3dbox(vec &o, float tofloor, float toceil, float xradius, float yradius = 0);

// octa
extern cube *newcubes(uint face = F_EMPTY, int mat = MAT_AIR);
extern cubeext *growcubeext(cubeext *ext, int maxverts);
extern void setcubeext(cube &c, cubeext *ext);
extern cubeext *newcubeext(cube &c, int maxverts = 0, bool init = true);
extern void getcubevector(cube &c, int d, int x, int y, int z, ivec &p);
extern void setcubevector(cube &c, int d, int x, int y, int z, const ivec &p);
extern int familysize(const cube &c);
extern void freeocta(cube *c);
extern void discardchildren(cube &c, bool fixtex = false, int depth = 0);
extern void optiface(uchar *p, cube &c);
extern void validatec(cube *c, int size = 0);
extern bool isvalidcube(const cube &c);
extern ivec lu;
extern int lusize;
extern cube &lookupcube(int tx, int ty, int tz, int tsize = 0, ivec &ro = lu, int &rsize = lusize);
extern const cube *neighbourstack[32];
extern int neighbourdepth;
extern const cube &neighbourcube(const cube &c, int orient, int x, int y, int z, int size, ivec &ro = lu, int &rsize = lusize);
extern void resetclipplanes();
extern int getmippedtexture(const cube &p, int orient);
extern void forcemip(cube &c, bool fixtex = true);
extern bool subdividecube(cube &c, bool fullcheck=true, bool brighten=true);
extern void edgespan2vectorcube(cube &c);
extern int faceconvexity(const ivec v[4]);
extern int faceconvexity(const ivec v[4], int &vis);
extern int faceconvexity(const vertinfo *verts, int numverts, int size);
extern int faceconvexity(const cube &c, int orient);
extern void calcvert(const cube &c, int x, int y, int z, int size, ivec &vert, int i, bool solid = false);
extern void calcvert(const cube &c, int x, int y, int z, int size, vec &vert, int i, bool solid = false);
extern uint faceedges(const cube &c, int orient);
extern bool collapsedface(const cube &c, int orient);
extern bool touchingface(const cube &c, int orient);
extern bool flataxisface(const cube &c, int orient);
extern bool collideface(const cube &c, int orient);
extern int genclipplane(const cube &c, int i, vec *v, plane *clip);
extern void genclipplanes(const cube &c, int x, int y, int z, int size, clipplanes &p, bool collide = true);
extern bool visibleface(const cube &c, int orient, int x, int y, int z, int size, ushort mat = MAT_AIR, ushort nmat = MAT_AIR, ushort matmask = MATF_VOLUME);
extern int classifyface(const cube &c, int orient, int x, int y, int z, int size);
extern int visibletris(const cube &c, int orient, int x, int y, int z, int size, ushort nmat = MAT_ALPHA, ushort matmask = MAT_ALPHA);
extern int visibleorient(const cube &c, int orient);
extern void genfaceverts(const cube &c, int orient, ivec v[4]);
extern int calcmergedsize(int orient, const ivec &co, int size, const vertinfo *verts, int numverts);
extern void invalidatemerges(cube &c, const ivec &co, int size, bool msg);
extern void calcmerges();

extern int mergefaces(int orient, facebounds *m, int sz);
extern void mincubeface(const cube &cu, int orient, const ivec &o, int size, const facebounds &orig, facebounds &cf, ushort nmat = MAT_AIR, ushort matmask = MATF_VOLUME);

static inline cubeext &ext(cube &c)
{
    return *(c.ext ? c.ext : newcubeext(c));
}

// ents
extern char *entname(entity &e);
extern bool haveselent();
extern undoblock *copyundoents(undoblock *u);
extern void pasteundoents(undoblock *u);

// octaedit
extern void cancelsel();
extern void rendertexturepanel(int w, int h);
extern void addundo(undoblock *u);
extern void commitchanges(bool force = false);
extern void rendereditcursor();
extern void tryedit();

// octarender
extern vector<tjoint> tjoints;

extern ushort encodenormal(const vec &n);
extern vec decodenormal(ushort norm);
extern void reduceslope(ivec &n);
extern void findtjoints();
extern void octarender();
extern void allchanged(bool load = false);
extern void clearvas(cube *c);
extern vtxarray *newva(int x, int y, int z, int size);
extern void destroyva(vtxarray *va, bool reparent = true);
extern bool readva(vtxarray *va, ushort *&edata, vertex *&vdata);
extern void updatevabb(vtxarray *va, bool force = false);
extern void updatevabbs(bool force = false);

// renderva
extern void visiblecubes(bool cull = true);
extern void setvfcP(float z = -1, const vec &bbmin = vec(-1, -1, -1), const vec &bbmax = vec(1, 1, 1));
extern void savevfcP();
extern void restorevfcP();
extern void rendergeom(float causticspass = 0, bool fogpass = false);
extern void renderalphageom(bool fogpass = false);
extern void rendermapmodels();
extern void renderreflectedgeom(bool causticspass = false, bool fogpass = false);
extern void renderreflectedmapmodels();
extern void renderoutline();
extern bool rendersky(bool explicitonly = false);

extern bool isfoggedsphere(float rad, const vec &cv);
extern int isvisiblesphere(float rad, const vec &cv);
extern bool bboccluded(const ivec &bo, const ivec &br);
extern occludequery *newquery(void *owner);
extern bool checkquery(occludequery *query, bool nowait = false);
extern void resetqueries();
extern int getnumqueries();
extern void drawbb(const ivec &bo, const ivec &br, const vec &camera = camera1->o);

extern int oqfrags;

#define startquery(query) do { glBeginQuery_(GL_SAMPLES_PASSED_ARB, ((occludequery *)(query))->id); } while(0)
#define endquery(query) do { glEndQuery_(GL_SAMPLES_PASSED_ARB); } while(0)

// dynlight

extern void updatedynlights();
extern int finddynlights();
extern void calcdynlightmask(vtxarray *va);
extern int setdynlights(vtxarray *va);
extern bool getdynlight(int n, vec &o, float &radius, vec &color);

// material

extern int showmat;

extern int findmaterial(const char *name);
extern const char *findmaterialname(int mat);
extern const char *getmaterialdesc(int mat, const char *prefix = "");
extern void genmatsurfs(const cube &c, int cx, int cy, int cz, int size, vector<materialsurface> &matsurfs);
extern void rendermatsurfs(materialsurface *matbuf, int matsurfs);
extern void rendermatgrid(materialsurface *matbuf, int matsurfs);
extern int optimizematsurfs(materialsurface *matbuf, int matsurfs);
extern void setupmaterials(int start = 0, int len = 0);
extern void rendermaterials();
extern int visiblematerial(const cube &c, int orient, int x, int y, int z, int size, ushort matmask = MATF_VOLUME);

// water
extern int refracting, refractfog;
extern bvec refractcolor;
extern bool reflecting, fading, fogging;
extern float reflectz;
extern int reflectdist, vertwater, waterrefract, waterreflect, waterfade, caustics, waterfallrefract;

#define GETMATIDXVAR(name, var, type) \
    type get##name##var(int mat) \
    { \
        switch(mat&MATF_INDEX) \
        { \
            default: case 0: return name##var; \
            case 1: return name##2##var; \
            case 2: return name##3##var; \
            case 3: return name##4##var; \
        } \
    }

extern const bvec &getwatercolor(int mat);
extern const bvec &getwaterfallcolor(int mat);
extern int getwaterfog(int mat);
extern const bvec &getlavacolor(int mat);
extern int getlavafog(int mat);
extern const bvec &getglasscolor(int mat);

extern void cleanreflections();
extern void queryreflections();
extern void drawreflections();
extern void renderwater();
extern void renderlava(const materialsurface &m, Texture *tex, float scale);
extern void loadcaustics(bool force = false);
extern void preloadwatershaders(bool force = false);

// glare
extern bool glaring;

extern void drawglaretex();
extern void addglare();

// depthfx
extern bool depthfxing;

extern void drawdepthfxtex();

// server
extern vector<const char *> gameargs;

extern void initserver(bool listen, bool dedicated);
extern void cleanupserver();
extern void serverslice(bool dedicated, uint timeout);
extern void updatetime();

extern ENetSocket connectmaster(bool wait);
extern void localclienttoserver(int chan, ENetPacket *);
extern void localconnect();
extern bool serveroption(char *opt);

// serverbrowser
extern bool resolverwait(const char *name, ENetAddress *address);
extern int connectwithtimeout(ENetSocket sock, const char *hostname, const ENetAddress &address);
extern void addserver(const char *name, int port = 0, const char *password = NULL, bool keep = false);
extern void writeservercfg();

// client
extern void localdisconnect(bool cleanup = true);
extern void localservertoclient(int chan, ENetPacket *packet);
extern void connectserv(const char *servername, int port, const char *serverpassword);
extern void abortconnect();
extern void clientkeepalive();

// command
extern hashset<ident> idents;
extern int identflags;

extern void clearoverrides();
extern void writecfg(const char *name = NULL);

extern void checksleep(int millis);
extern void clearsleep(bool clearoverrides = true);

// console
extern void keypress(int code, bool isdown, int cooked);
extern int rendercommand(int x, int y, int w);
extern int renderconsole(int w, int h, int abovehud);
extern void conoutf(const char *s, ...) PRINTFARGS(1, 2);
extern void conoutf(int type, const char *s, ...) PRINTFARGS(2, 3);
extern void resetcomplete();
extern void complete(char *s, const char *cmdprefix);
const char *getkeyname(int code);
extern const char *addreleaseaction(char *s);
extern void writebinds(stream *f);
extern void writecompletions(stream *f);

// main
enum
{
    NOT_INITING = 0,
    INIT_LOAD,
    INIT_RESET
};
extern int initing, numcpus;

enum
{
    CHANGE_GFX   = 1<<0,
    CHANGE_SOUND = 1<<1
};
extern bool initwarning(const char *desc, int level = INIT_RESET, int type = CHANGE_GFX);

extern bool grabinput, minimized;

extern void pushevent(const SDL_Event &e);
extern bool interceptkey(int sym);

extern float loadprogress;
extern void renderbackground(const char *caption = NULL, Texture *mapshot = NULL, const char *mapname = NULL, const char *mapinfo = NULL, bool restore = false, bool force = false);
extern void renderprogress(float bar, const char *text, GLuint tex = 0, bool background = false);

extern void getfps(int &fps, int &bestdiff, int &worstdiff);
extern void swapbuffers(bool overlay = true);
extern int getclockmillis();

// menu
extern void menuprocess();
extern void addchange(const char *desc, int type);
extern void clearchanges(int type);

// physics
extern void mousemove(int dx, int dy);
extern bool pointincube(const clipplanes &p, const vec &v);
extern bool overlapsdynent(const vec &o, float radius);
extern void rotatebb(vec &center, vec &radius, int yaw);
extern float shadowray(const vec &o, const vec &ray, float radius, int mode, extentity *t = NULL);
struct ShadowRayCache;
extern ShadowRayCache *newshadowraycache();
extern void freeshadowraycache(ShadowRayCache *&cache);
extern void resetshadowraycache(ShadowRayCache *cache);
extern float shadowray(ShadowRayCache *cache, const vec &o, const vec &ray, float radius, int mode, extentity *t = NULL);

// world

extern vector<int> outsideents;

extern void entcancel();
extern void entitiesinoctanodes();
extern void attachentities();
extern void freeoctaentities(cube &c);
extern bool pointinsel(const selinfo &sel, const vec &o);

extern void resetmap();
extern void startmap(const char *name);

// rendermodel
struct mapmodelinfo { string name; model *m; };

extern void findanims(const char *pattern, vector<int> &anims);
extern void loadskin(const char *dir, const char *altdir, Texture *&skin, Texture *&masks);
extern mapmodelinfo *getmminfo(int i);
extern void startmodelquery(occludequery *query);
extern void endmodelquery();
extern void preloadmodelshaders();
extern void preloadusedmapmodels(bool msg = false, bool bih = false);

static inline model *loadmapmodel(int n)
{
    extern vector<mapmodelinfo> mapmodels;
    if(mapmodels.inrange(n))
    {
        model *m = mapmodels[n].m;
        return m ? m : loadmodel(NULL, n);
    }
    return NULL;
}

// renderparticles
extern void particleinit();
extern void clearparticles();
extern void clearparticleemitters();
extern void seedparticles();
extern void updateparticles();
extern void renderparticles(bool mainpass = false);
extern bool printparticles(extentity &e, char *buf);

// decal
extern void initdecals();
extern void cleardecals();
extern void renderdecals(bool mainpass = false);

// blob

enum
{
    BLOB_STATIC = 0,
    BLOB_DYNAMIC
};

extern int showblobs;

extern void initblobs(int type = -1);
extern void resetblobs();
extern void renderblob(int type, const vec &o, float radius, float fade = 1);
extern void flushblobs();

// rendersky
extern int explicitsky;
extern double skyarea;

extern void drawskybox(int farplane, bool limited);
extern bool limitsky();

// 3dgui
extern void g3d_render();
extern bool g3d_windowhit(bool on, bool act);

// menus
extern int mainmenu;

extern void clearmainmenu();
extern void g3d_mainmenu();

// sound
extern void clearmapsounds();
extern void checkmapsounds();
extern void updatesounds();
extern void preloadmapsounds();

extern void initmumble();
extern void closemumble();
extern void updatemumble();

// grass
extern void generategrass();
extern void rendergrass();

// blendmap
extern int blendpaintmode;

struct BlendMapCache;
extern BlendMapCache *newblendmapcache();
extern void freeblendmapcache(BlendMapCache *&cache);
extern bool setblendmaporigin(BlendMapCache *cache, const ivec &o, int size);
extern bool hasblendmap(BlendMapCache *cache);
extern uchar lookupblendmap(BlendMapCache *cache, const vec &pos);
extern void resetblendmap();
extern void enlargeblendmap();
extern void shrinkblendmap(int octant);
extern void optimizeblendmap();
extern void stoppaintblendmap();
extern void trypaintblendmap();
extern void renderblendbrush(GLuint tex, float x, float y, float w, float h);
extern void renderblendbrush();
extern bool loadblendmap(stream *f, int info);
extern void saveblendmap(stream *f);
extern uchar shouldsaveblendmap();

// recorder

namespace recorder
{
    extern void stop();
    extern void capture(bool overlay = true);
    extern void cleanup();
}

#endif

#endif

