/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_TOPOLOGY_MULTILEVELHEXAHEDRONSETTOPOLOGYCONTAINER_H
#define SOFA_COMPONENT_TOPOLOGY_MULTILEVELHEXAHEDRONSETTOPOLOGYCONTAINER_H

#include <sofa/component/topology/HexahedronSetTopologyContainer.h>
#include <sofa/component/topology/HexahedronData.h>
#include <sofa/core/componentmodel/topology/Topology.h>
#include <sofa/defaulttype/Vec.h>
#include <set>

namespace sofa
{
namespace core
{
namespace componentmodel
{
namespace topology
{
class TopologyChange;
}
}
}

namespace component
{

namespace topology
{
class MultilevelHexahedronSetTopologyModifier;

using sofa::defaulttype::Vec;
using sofa::core::componentmodel::topology::TopologyChange;

class SOFA_COMPONENT_CONTAINER_API MultilevelHexahedronSetTopologyContainer : public HexahedronSetTopologyContainer
{
    friend class MultilevelHexahedronSetTopologyModifier;

public:
    typedef Vec<3,int>			Vec3i;

public:
    MultilevelHexahedronSetTopologyContainer();

    MultilevelHexahedronSetTopologyContainer(const helper::vector< Hexahedron > &hexahedra);

    virtual ~MultilevelHexahedronSetTopologyContainer();

    virtual void init();

    virtual void clear();

    virtual void loadFromMeshLoader(sofa::component::MeshLoader* loader);

    void getHexaNeighbors(const unsigned int hexaId,
            helper::vector<unsigned int> &neighbors);

    void getHexaFaceNeighbors(const unsigned int hexaId,
            const unsigned int faceId,
            helper::vector<unsigned int> &neighbors);

    void getHexaVertexNeighbors(const unsigned int hexaId,
            const unsigned int vertexId,
            helper::vector<unsigned int> &neighbors);

    void addTopologyChangeFine(const TopologyChange *topologyChange)
    {
        m_changeListFine.push_back(topologyChange);
    }

    void resetTopologyChangeListFine()
    {
        for(std::list<const TopologyChange *>::iterator it = m_changeListFine.begin();
            it != m_changeListFine.end(); ++it)
        {
            delete (*it);
        }
        m_changeListFine.clear();
    }

    std::list<const TopologyChange *>::const_iterator firstChangeFine() const
    {
        return m_changeListFine.begin();
    }

    std::list<const TopologyChange *>::const_iterator lastChangeFine() const
    {
        return m_changeListFine.end();
    }

    const std::list<const TopologyChange *>& getChangeListFine() const
    {
        return m_changeListFine;
    }

    int getLevel() const {return _level.getValue();}

    const Vec3i& getFineResolution() const { return _fineResolution; }

    const Vec3i& getCoarseResolution() const { return _coarseResolution; }

    bool getHexaContainsPosition(const unsigned int hexaId, const defaulttype::Vector3& baryC) const;

    const Vec3i& getHexaIdxInCoarseRegularGrid(const unsigned int hexaId) const;
    int getHexaIdInCoarseRegularGrid(const unsigned int hexaId) const;

    const Vec3i& getHexaIdxInFineRegularGrid(const unsigned int hexaId) const;
    int getHexaIdInFineRegularGrid(const unsigned int hexaId) const;

    // gets a vector of fine hexas inside a specified coarse hexa
    int getHexaChildren(const unsigned int hexaId, helper::vector<unsigned int>& children) const;

    // gets a coarse hexa for a specified fine hexa
    int getHexaParent(const unsigned int hexaId) const;

    int getHexaInFineRegularGrid(const Vec3i& id) const;

private:
    void setCoarseResolution(const Vec3i& res) { _coarseResolution = res; }

    void connectionToNodeAdjacency(const Vec3i& connection, std::map<unsigned int, unsigned int>& nodeMap) const;

    class Component
    {
    public:
        Component(const Vec3i& id, const std::set<Vec3i>& voxels);
        virtual ~Component();

        bool isEmpty() const;

        bool isConnected(const Component* other) const;
        bool getConnection(const Component* other, Vec3i& connection) const;
        bool merge(Component* other);

        void split(std::set<Component*>& newComponents);

        void clear();
        void removeVoxels(const std::set<Vec3i>& voxels);

        bool hasVoxel(const Vec3i& voxel) const;

        const Vec3i& getVoxelId() const {return _id;}

        bool isStronglyConnected() const;

        int getLevel() const;

        inline friend std::ostream& operator<< (std::ostream& out, const Component* /*t*/)
        {
            return out;
        }

        inline friend std::istream& operator>>(std::istream& in, Component* /*t*/)
        {
            return in;
        }

    private:
        Component(const Vec3i& id);
        bool isConnected(const std::set<Vec3i>&, const Vec3i&) const;

    public:
        Component*				_parent;
        std::set<Component*>	_children;
        std::set<Vec3i>			_voxels;

    private:
        Vec3i					_id;		// voxel id in the corresponding level
    };

private:
    Data<int> _level;

    std::list<const TopologyChange *>	m_changeListFine;

    HexahedronData<Component*>		_coarseComponents;	// map between hexahedra and components - coarse
    HexahedronData<Component*>		_fineComponents;	// map between hexahedra and components - fine

    // the fine mesh must be a regular grid - store its parameters here
    Vec3i	_fineResolution;		// width, height, depth (number of hexa in each direction)
    Vec3i	_coarseResolution;

    sofa::helper::vector<Component*>	_fineComponentInRegularGrid;
};

/** notifies change in the multilevel structure other than adding or removing coarse hexas */
class MultilevelModification : public core::componentmodel::topology::TopologyChange
{
public:
    static const int MULTILEVEL_MODIFICATION = core::componentmodel::topology::TOPOLOGYCHANGE_LASTID + 1;

    MultilevelModification(const sofa::helper::vector<unsigned int> _tArray, const sofa::helper::vector<unsigned int> removedFineHexahedraArray)
        : core::componentmodel::topology::TopologyChange((core::componentmodel::topology::TopologyChangeType) MULTILEVEL_MODIFICATION)
        , modifiedHexahedraArray(_tArray)
        , _removedFineHexahedraArray(removedFineHexahedraArray)
    {}

    const sofa::helper::vector<unsigned int> &getArray() const
    {
        return modifiedHexahedraArray;
    }

    const sofa::helper::vector<unsigned int> &getRemovedFineHexahedraArray() const
    {
        return _removedFineHexahedraArray;
    }

    unsigned int getNbModifiedHexahedra() const
    {
        return modifiedHexahedraArray.size();
    }

public:
    sofa::helper::vector<unsigned int> modifiedHexahedraArray;
    sofa::helper::vector<unsigned int> _removedFineHexahedraArray;

};


} // namespace topology

} // namespace component

} // namespace sofa

#endif
