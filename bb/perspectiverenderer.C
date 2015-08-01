#include "perspectiverenderer.h"
#include "player.h"
#include <algorithm>     // ugh, this baby's big

/*-------------------------------------------------------------------------*/
// Draw the 3-D view for this player.

void DrawView(vhclGob* vhcl, struct bbGfxTarget& bbgt, struct gfxTarget& gt, bool usingHqView, bool shadowsOn, int teamNum, team teams[], int playerNum, bool doubleBuff, hrznGob *horizon)
{
  gobList& gobs = *gob::gobs;

  tmtrx viewPos= vhcl->ViewPos(); // both cartesian & angular position
  pt3d eyePos= viewPos.Cart();      // just cartesian position

  if (usingHqView) {
    eyePos= teams[teamNum].hq->ViewPos().Cart();
    pt3d vhclCenter= pt3d(0,0,vhcl->Shape()->Box().Center().z) >> vhcl->WorldPos();
    viewPos= tcomp(eyePos,(vhclCenter -eyePos).Ang3d());
  }

  pt3d  frustrumNormals[4];
  frustrumNormals[0]= (pt3d(1, 2, 0) >> viewPos) -viewPos.Cart();
  frustrumNormals[1]= (pt3d(1,-2, 0) >> viewPos) -viewPos.Cart();
  frustrumNormals[2]= (pt3d(1, 0, 4) >> viewPos) -viewPos.Cart();
  frustrumNormals[3]= (pt3d(1, 0,-4) >> viewPos) -viewPos.Cart();
  frustrumNormals[0].Normalize();
  frustrumNormals[1].Normalize();
  frustrumNormals[2].Normalize();
  frustrumNormals[3].Normalize();

  // compute the world-to-view transformation
  tmtrx worldToView= viewPos.Inverse();
  // scale worldToView to do some of the 3D->2D screen projection
  forii(4)
    for (int j= 1; j <=2; j++)
      worldToView[i][j] *= (-bbgt.gfxSize.x);

  if (doubleBuff) gt.DoubleBufferBegin();

  if (not horizon->Solid()) {
    gt.SetForeground(gt.White());
    gt.FillRectangle(pt2d(0,0),bbgt.gfxSize);
    gt.SetForeground(gt.Black());
  }

  horizon->SetWorldPos(viewPos);

  // sort all the non-part gobs by distance from viewer
  gobList::iterator gi;
  vector<gobRef> refs;
  for_(gi,gobs)
    refs.push_back(gobRef(*gi,(**gi).DistFromViewer(eyePos)));
  stable_sort(refs.begin(),refs.end());

  // now insert gob parts into the list, in correct drawing order
  gobList allGobs;
  vector<gobRef>::iterator gr;
  for_(gr,refs)
    (**gr).GetViewOrder(eyePos,allGobs);

  // draw the horizon, then the playfield (they're the last two gobRefs)
  gi= allGobs.end();
  (**(--gi)).Draw(bbgt,worldToView,playerNum,eyePos); // horizon
  (**(--gi)).Draw(bbgt,worldToView,playerNum,eyePos); // playfield

  // Next, draw any shadows
  ulong tempColors[totalColors];

  if (shadowsOn) {
    fori(totalColors)
      tempColors[i]= gt.Colors()[c_gray20];
    
    gobList::iterator savedGi= gi;
    while (gi != allGobs.begin()) {
      gi--;
      rgn3 *shadow= (**gi).Shadow();
      if (shadow) {
	// project flattened shadow onto the ground!
	tmtrx pos= (**gi).WorldPos();
	pos[0][2] *= 1e-6;   // I'm worried about making a "singular" mtrx
	pos[1][2] *= 1e-6;   // here, so I don't quite zero this column...
	pos[2][2] *= 1e-6;
	pos[3][2]= 0;
	//pos.UnitScaling()= false;
	if (shadow->InFrustrum(eyePos,frustrumNormals,pos)) {
	  view v;
	  v.Update(shadow,pos*worldToView);
	  gt.SetForeground(gt.Colors()[c_gray20]);
	  shadow->Draw(v.pts, gt, eyePos<<pos, bbgt.gfxSize, //eyePos<<pos -PAH
		       (**gi).Solid(), tempColors);
	}
      }
    }
    gi= savedGi;
  }

  // Finally, draw the gobs themselves

  vhcl->EyeIsInside(not usingHqView);

  while (gi != allGobs.begin()) {
    gi--;
    if ((**gi).Shape()->InFrustrum(eyePos,frustrumNormals,(**gi).WorldPos())) {
      ulong *colors= gt.Colors();
      if ((**gi).TeamNum() != -1) {
	colors= tempColors;
	fori(totalColors)
	  tempColors[i]= gt.Colors()[teams[(**gi).TeamNum()].colorNums[i]];
      }

      (**gi).Draw(bbgt,worldToView,playerNum,eyePos,colors);
    }
  }
  
  vhcl->EyeIsInside(false);
}

PerspectiveRenderer::PerspectiveRenderer()
{
}

PerspectiveRenderer::~PerspectiveRenderer()
{
}

void PerspectiveRenderer::render(struct gfxTarget* gfxtarget)
{
  DrawView(thePlayer->Vhcl(), thePlayer->gt, *gfxtarget, thePlayer->usingHqView, thePlayer->shadowsOn, teamIndex, teams, playerIndex, *doubleBuffer, *horizon);
}

