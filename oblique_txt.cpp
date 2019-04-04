//********************************************************************************
// Function to draw a text string with an oblique or slant angle.
// Call this function in places where you would normally call CDC::TextOut();
// Text rotation must be set in dc using lfEscapement & lfOrientation of LOGFONT. 
// Text alignment mode must be set in dc using SetTextAlign().
// oblique is the oblique angle, in 1/10th degrees. + if the text slants to the right
// 
void HELPDIAG::ObliqueTextOut( CDC *dc, int oblique, int x, int y, const CString &DrwText )
{
  ASSERT( oblique>5 );     // Warn if <0.5 degree. Unit for "oblique" may be wrong
//
//--- Get LOGFONT and text rotation and convert to radians
//
  LOGFONT lgf;
  dc->GetCurrentFont()->GetLogFont( &lgf );
  double txtRotate = (double) lgf.lfEscapement*3.1415926538 / 1800.0;  // from 1/10th degrees to radian.
  double txtOblique = oblique*3.1415926538 / 1800.0;  // from 1/10th degrees to radian.
//
//--- Size of the text box
//
  TEXTMETRIC tm; 
  dc->GetTextMetrics( &tm );
  CSize ts = dc->GetTextExtent( DrwText );
  ts.cy = tm.tmHeight;
//
//--- Set transformation
//
  XFORM xForm;
  xForm.eDx = (float) x;
  xForm.eDy = (float) y;
  xForm.eM11 = (float) cos(txtRotate);
  xForm.eM21 = (float) (sin(txtRotate) - tan(txtOblique)*cos(txtRotate));
  xForm.eM12 = (float) -sin(txtRotate);
  xForm.eM22 = (float) (cos(txtRotate) + tan(txtOblique)*sin(txtRotate));

  SetGraphicsMode( dc->m_hDC, GM_ADVANCED );
  SetWorldTransform( dc->m_hDC, &xForm );
//
//--- Reset font and draw
//
  lgf.lfOrientation -= lgf.lfEscapement;
  lgf.lfEscapement = 0;
  CFont horFont;
  horFont.CreateFontIndirect( &lgf );
  CFont *OldFont = dc->SelectObject( &horFont );

  dc->TextOut( 0,0, DrwText );
//
//--- Set back to identity transformation
//
  ModifyWorldTransform( dc->m_hDC, &xForm, MWT_IDENTITY );
  SetGraphicsMode( dc->m_hDC, GM_COMPATIBLE );

  dc->SelectObject( OldFont );
}
