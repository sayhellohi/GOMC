/*******************************************************************************
GPU OPTIMIZED MONTE CARLO (GOMC) 2.31
Copyright (C) 2018  GOMC Group
A copy of the GNU General Public License can be found in the COPYRIGHT.txt
along with this program, also can be found at <http://www.gnu.org/licenses/>.
********************************************************************************/
#ifndef TRIALMOL_H
#define TRIALMOL_H

#include "EnergyTypes.h"
#include "XYZArray.h"
#include "TransformMatrix.h"
#include "BasicTypes.h"
#include "MoleculeKind.h"
#include "GeomLib.h"

class MoleculeKind;
class BoxDimensions;
class CalculateEnergy;

//! Class for keeping track of part-built molecules during CBMC
namespace cbmc
{

class TrialMol
{
public:
  //!Construct TrialMol of kind k to be evaluated in box with axes ax.
  TrialMol(const MoleculeKind& k, const BoxDimensions& ax,
           uint box);
  //!Construct invalid default TrialMol
  TrialMol();

  TrialMol(const TrialMol& other);
  TrialMol& operator=(TrialMol other);
  friend void swap(TrialMol& a, TrialMol& b);

  //!True if this has been initialized to be valid
  bool IsValid() const
  {
    return (atomBuilt != NULL);
  }

  void AddAtom(uint index, const XYZ& position);
  void SetAtomCoords(uint index, const XYZ& loc);

  void AddEnergy(const Energy& energy)
  {
    en += energy;
  }

  //!Confirms that atom at index i has been built (used for oldMols)
  void ConfirmOldAtom(uint i);

  //!Sets an orthonormal basis for coordinate conversion.
  /*!\param p1 Index of particle new additions will be bonded to
   * \param p2 Index of particle that will be in angles with new additions
   * \param p3 Index of particle against which dihedrals will be measured
   */
  void SetBasis(uint p1, uint p2, uint p3);

  //!Sets an orthonormal basis for coordinate conversion.
  /*!\param p1 Index of particle new additions will be bonded to
   * \param p2 Index of particle that will be in angles with new additions
   */
  void SetBasis(uint p1, uint p2);

  //!Shifts the current basis to the position of p1, but does not rotate it.
  void ShiftBasis(uint p1);

  //!Shifts the current basis to the XYZ coordinate.
  void ShiftBasis(XYZ cent);

  //!Resets basis to box coordinate system
  void ResetBasis();

  //!Returns wrapped rectangular coordinates of a candidate position;
  XYZ GetRectCoords(double bond, double theta, double phi) const;

  XYZ RawRectCoords(double bond, double theta, double phi) const;

  // Returns the dihedral angle between two positions
  /* \param theta1 Theta spherical coordinate of first position
   * \param theta2 Theta spherical coordinate of second position
   * \param interior The interor angle between the positions
   */
  static double PhiBetweenAngles(double theta1, double theta2,
                                 double interior);

  //!Return angle in radians between confirmed atoms a, b and c
  double GetTheta(uint a, uint b, uint c) const;

  //!Calculates theta and phi coords for atom in the current basis
  //!centered on lastAtom. theta in [0, pi], phi in (-pi, pi]
  void OldThetaAndPhi(uint atom, uint lastAtom,
                      double& theta, double& phi) const;

  //!calculate distance between atoms belong to specified angle
  double AngleDist(const double b1, const double b2, const double theta);

  //!calculate distance between atoms belong to specified dihedral
  double DihedDist(const double b1, const double b2, const double b3,
                   const double theta1, const double theta2,
                   const double phi);

  //!calculate distance between two atom in oldMol
  double OldDistSq(const uint atom, const uint lastAtom);

  //calculate min image distance between a and b
  double DistSq(const XYZ& a, const XYZ& b);

  const Energy& GetEnergy() const
  {
    return en;
  }
  double GetWeight() const
  {
    return totalWeight;
  }
  void SetWeight(double w)
  {
    totalWeight = w;
  }
  void MultWeight(double w)
  {
    totalWeight *= w;
  }

  uint GetBox() const
  {
    return box;
  }
  const BoxDimensions& GetAxes() const
  {
    return *axes;
  }
  const MoleculeKind& GetKind() const
  {
    return *kind;
  }

  bool OneFour() const
  {
    return kind->oneFour;
  }

  //!Returns reference to coordinates of TrialMol.
  const XYZArray& GetCoords() const
  {
    return tCoords;
  }

  //!Returns position of atom i (undefined if it doesn't exist yet)
  XYZ AtomPosition(const uint atom) const
  {
    return tCoords.Get(atom);
  }

  //!Copies 1 molecule's worth of coordinates from coords[start] onwards
  void SetCoords(const XYZArray& coords, uint start);

  bool AtomExists(uint index) const
  {
    return atomBuilt[index];
  }

  //Used in MEMC move
  void SetSeed(const XYZ& coords, const XYZ& cav, const bool inCav,
	       const bool fixCOM, const bool rotBB);
  void SetSeed(const bool inCav, const bool fixCOM, const bool rotBB);
  void SetBackBone(const uint bb[2]);
  XYZ Transform(const XYZ& a) {return geom::Transform(cavMatrix, a);}
  void TransposeMatrix(XYZArray &invMatrix)
  {return geom::TransposeMatrix(invMatrix, cavMatrix);}
  bool HasCav() const {return comInCav;}
  bool COMFix() const {return comFix;}
  bool RotateBB() const {return rotateBB;}
  void SetCavMatrix(const XYZArray& matrix);
  XYZ GetCavityCenter() const {return cavityCenter;}
  XYZ GetCavity() const {return cavity;}
  //return unwrap com of tcoords so tcoords must be set
  XYZ GetCOM();
  uint GetAtomBB(const uint i) const { return backbone[i];}

  ~TrialMol();

private:
  friend class CalculateEnergy;

  const MoleculeKind* kind;
  const BoxDimensions* axes;
  uint box;
  XYZArray tCoords, cavMatrix;
  Energy en;
  double totalWeight;
  RotationMatrix growthToWorld;
  RotationMatrix worldToGrowth;
  XYZ basisPoint;
  XYZ cavityCenter, cavity; //The center and cavity dimensions
  uint backbone[2];
  bool comInCav, comFix, rotateBB;
  bool* atomBuilt;

};
}




#endif
