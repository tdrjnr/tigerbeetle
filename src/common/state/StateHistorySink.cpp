/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstdint>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <yajl_gen.h>
#include <delorean/BasicTypes.hpp>
#include <delorean/interval/AbstractInterval.hpp>
#include <delorean/interval/Int32Interval.hpp>
#include <delorean/interval/Uint32Interval.hpp>
#include <delorean/interval/Int64Interval.hpp>
#include <delorean/interval/Uint64Interval.hpp>
#include <delorean/interval/Float32Interval.hpp>
#include <delorean/interval/QuarkInterval.hpp>
#include <delorean/interval/NullInterval.hpp>
#include <delorean/HistoryFileSink.hpp>

#include <common/state/AbstractStateNodeVisitor.hpp>
#include <common/state/StateValueType.hpp>
#include <common/state/AbstractStateValue.hpp>
#include <common/state/StateHistorySink.hpp>
#include <common/state/CurrentState.hpp>
#include <common/state/QuarkStateValue.hpp>
#include <common/ex/WrongQuark.hpp>

namespace bfs = boost::filesystem;

namespace tibee
{
namespace common
{

/**
 * State node visitor that counts the number of active nodes (not null).
 *
 * @author Philippe Proulx
 */
class StateNodeCounterVisitor :
    public AbstractStateNodeVisitor
{
public:
    StateNodeCounterVisitor() :
        _count {0}
    {
    }

    std::size_t getCount() const
    {
        return _count;
    }

private:
    void visitEnterImpl(quark_t quark, const StateNode& node)
    {
        if (node) {
            _count++;
        }
    }

private:
    std::size_t _count;
};

/**
 * State node visitor that creates a JSON string (using YAJL)
 * representing the state tree it visits.
 *
 * Each node in the tree has the the "id" field, which is its numeric,
 * unique node ID, and an optional field "children", which is a
 * dictionary of subpath to node.
 *
 * The root of this tree is always considered to have no name.
 *
 * @author Philippe Proulx
 */
class TreeToJsonStateNodeVisitor :
    public AbstractStateNodeVisitor
{
public:
    /**
     * Builds a tree to JSON state node visitor.
     *
     * An associated state history sink is given, to accept queries
     * of quarks to subpaths.
     *
     * @param yajlGen          YAJL generator to use for writing JSON
     * @param stateHistorySink Associated state history sink
     */
    TreeToJsonStateNodeVisitor(::yajl_gen yajlGen,
                               const StateHistorySink* stateHistorySink) :
        _yajlGen {yajlGen},
        _stateHistorySink {stateHistorySink}
    {
    }

private:
    void visitEnterImpl(quark_t quark, const StateNode& node)
    {
        if (quark != 0xffffffff) {
            // not root: output child node subpath
            const auto& subpath = _stateHistorySink->getSubpathString(quark);

            ::yajl_gen_string(_yajlGen,
                              reinterpret_cast<const unsigned char*>(subpath.c_str()),
                              subpath.length());
        }

        // open map for this node
        ::yajl_gen_map_open(_yajlGen);

        // write node ID
        static const std::string KEY_ID {"id"};

        ::yajl_gen_string(_yajlGen,
                          reinterpret_cast<const unsigned char*>(KEY_ID.c_str()),
                          KEY_ID.length());
        ::yajl_gen_integer(_yajlGen, node.getId());

        // open node children map (if it has any)
        if (node.getAllChildrenCount() > 0) {
            static const std::string KEY_CHILDREN {"children"};

            ::yajl_gen_string(_yajlGen,
                              reinterpret_cast<const unsigned char*>(KEY_CHILDREN.c_str()),
                              KEY_CHILDREN.length());
            ::yajl_gen_map_open(_yajlGen);
        }
    }

    void visitLeaveImpl(quark_t quark, const StateNode& node)
    {
        // close children map (if it has any)
        if (node.getAllChildrenCount() > 0) {
            ::yajl_gen_map_close(_yajlGen);
        }

        // close node map
        ::yajl_gen_map_close(_yajlGen);
    }

private:
    // YAJL generator
    ::yajl_gen _yajlGen;

    // associated state history sink
    const StateHistorySink* _stateHistorySink;
};

StateHistorySink::StateHistorySink(const bfs::path& subpathStrDbPath,
                                   const bfs::path& valueStrDbPath,
                                   const boost::filesystem::path& nodesMapPath,
                                   const bfs::path& historyPath,
                                   timestamp_t beginTs) :
    _subpathStrDbPath {subpathStrDbPath},
    _valueStrDbPath {valueStrDbPath},
    _nodesMapPath {nodesMapPath},
    _historyPath {historyPath},
    _beginTs {beginTs},
    _ts {beginTs},
    _open {false},
    _nextPathQuark {0},
    _nextStrValueQuark {0},
    _nextNodeId {0},
    _currentState {this},
    _stateChangesCount {0}
{
    _intervalFileSink = std::unique_ptr<delo::HistoryFileSink> {
        new delo::HistoryFileSink
    };

    _null = NullStateValue::UP {new NullStateValue};

    this->initTranslators();
    this->open();
}

StateHistorySink::~StateHistorySink()
{
    this->close();
}

void StateHistorySink::initTranslators()
{
    auto unknownTranslator = [] (const StateNode&)
    {
        return nullptr;
    };

    auto nullTranslator = [this] (const StateNode& node)
    {
        return new delo::NullInterval {
            static_cast<delo::timestamp_t>(node.getBeginTs()),
            static_cast<delo::timestamp_t>(_ts),
            static_cast<delo::interval_key_t>(node.getId())
        };
    };

    auto sint32Translator = [this] (const StateNode& node)
    {
        auto interval = new delo::Int32Interval {
            static_cast<delo::timestamp_t>(node.getBeginTs()),
            static_cast<delo::timestamp_t>(_ts),
            static_cast<delo::interval_key_t>(node.getId())
        };

        interval->setValue(node.getValue().asSint32());

        return interval;
    };

    auto uint32Translator = [this] (const StateNode& node)
    {
        auto interval = new delo::Uint32Interval {
            static_cast<delo::timestamp_t>(node.getBeginTs()),
            static_cast<delo::timestamp_t>(_ts),
            static_cast<delo::interval_key_t>(node.getId())
        };

        interval->setValue(node.getValue().asUint32());

        return interval;
    };

    auto sint64Translator = [this] (const StateNode& node)
    {
        auto interval = new delo::Int64Interval {
            static_cast<delo::timestamp_t>(node.getBeginTs()),
            static_cast<delo::timestamp_t>(_ts),
            static_cast<delo::interval_key_t>(node.getId())
        };

        interval->setValue(node.getValue().asSint64());

        return interval;
    };

    auto uint64Translator = [this] (const StateNode& node)
    {
        auto interval = new delo::Uint64Interval {
            static_cast<delo::timestamp_t>(node.getBeginTs()),
            static_cast<delo::timestamp_t>(_ts),
            static_cast<delo::interval_key_t>(node.getId())
        };

        interval->setValue(node.getValue().asUint64());

        return interval;
    };

    auto float32Translator = [this] (const StateNode& node)
    {
        auto interval = new delo::Float32Interval {
            static_cast<delo::timestamp_t>(node.getBeginTs()),
            static_cast<delo::timestamp_t>(_ts),
            static_cast<delo::interval_key_t>(node.getId())
        };

        interval->setValue(node.getValue().asFloat32());

        return interval;
    };

    auto quarkTranslator = [this] (const StateNode& node)
    {
        auto interval = new delo::QuarkInterval {
            static_cast<delo::timestamp_t>(node.getBeginTs()),
            static_cast<delo::timestamp_t>(_ts),
            static_cast<delo::interval_key_t>(node.getId())
        };

        interval->setValue(node.getValue().asQuark());

        return interval;
    };

    // fill translators
    for (auto& translator : _translators) {
        translator = unknownTranslator;
    }

    _translators[static_cast<std::size_t>(StateValueType::SINT32)] = sint32Translator;
    _translators[static_cast<std::size_t>(StateValueType::UINT32)] = uint32Translator;
    _translators[static_cast<std::size_t>(StateValueType::SINT64)] = sint64Translator;
    _translators[static_cast<std::size_t>(StateValueType::UINT64)] = uint64Translator;
    _translators[static_cast<std::size_t>(StateValueType::FLOAT32)] = float32Translator;
    _translators[static_cast<std::size_t>(StateValueType::QUARK)] = quarkTranslator;
    _translators[static_cast<std::size_t>(StateValueType::NUL)] = nullTranslator;
}

void StateHistorySink::open()
{
    // open history sink
    _intervalFileSink->open(_historyPath);

    // reset stuff
    _ts = _beginTs;
    _subpathsDb.clear();
    _strValuesDb.clear();
    _nextPathQuark = 0;
    _nextStrValueQuark = 0;
    _stateChangesCount = 0;

    // create root node
    _root = this->buildStateNode();

    _open = true;
}

void StateHistorySink::close()
{
    // silently ignore if already closed
    if (!_open) {
        return;
    }

    // write all remaining state values as intervals
    // FIXME

    // write files
    _intervalFileSink->close();
    this->writeStringDb(_subpathsDb, _subpathStrDbPath);
    this->writeStringDb(_strValuesDb, _valueStrDbPath);
    this->writeNodesMap();

    // clear string databases
    _subpathsDb.clear();
    _strValuesDb.clear();

    // set as closed
    _open = false;
}

quark_t StateHistorySink::getQuark(StringDb& stringDb, const std::string& value,
                                   quark_t& nextQuark)
{
    // find in map
    auto it = stringDb.left.find(value);

    if (it == stringDb.left.end()) {
        // not found: insert it and return new quark
        stringDb.insert(StringDb::value_type {value, nextQuark});
        nextQuark++;

        return nextQuark - 1;
    } else {
        return it->second;
    }
}

const std::string& StateHistorySink::getQuarkString(const StringDb& stringDb,
                                                    quark_t quark)
{
    auto it = stringDb.right.find(quark);

    if (it == stringDb.right.end()) {
        throw ex::WrongQuark {quark};
    }

    return it->second;
}

quark_t StateHistorySink::getSubpathQuark(const std::string& subpath)
{
    return this->getQuark(_subpathsDb, subpath, _nextPathQuark);
}

quark_t StateHistorySink::getStringValueQuark(const std::string& value)
{
    return this->getQuark(_strValuesDb, value, _nextStrValueQuark);
}

const std::string& StateHistorySink::getSubpathString(quark_t quark) const
{
    return StateHistorySink::getQuarkString(_subpathsDb, quark);
}

const std::string& StateHistorySink::getStringValueString(quark_t quark) const
{
    return StateHistorySink::getQuarkString(_strValuesDb, quark);
}

void StateHistorySink::writeInterval(const StateNode& node)
{
    // state value
    const auto& stateValue = node.getValue();

    // if the node does not have any current state value, cancel
    if (!stateValue) {
        return;
    }

    // translate from state value to interval
    auto type = static_cast<std::size_t>(stateValue.getType());
    auto interval = _translators[type](node);

    // ignore if unknown state value
    if (!interval) {
        return;
    }

    // add to interval history
    //_intervalFileSink->addInterval(delo::AbstractInterval::UP {interval});
    std::cout << "would add " << *interval << std::endl;

    // update internal statistics
    _stateChangesCount++;
}

void StateHistorySink::writeStringDb(const StringDb& stringDb,
                                     const boost::filesystem::path& path)
{
    // open output file for writing
    bfs::ofstream output;

    output.open(path, std::ios::binary);

    // write all string/quark pairs
    for (const auto& stringQuarkPair : stringDb.left) {
        const auto& string = stringQuarkPair.first;
        auto quark = stringQuarkPair.second;

        // write string part
        output.write(string.c_str(), string.size() + 1);

        // align for quark
        output.seekp((output.tellp() + static_cast<long>(sizeof(quark) - 1)) & ~(sizeof(quark) - 1));

        // write quark
        output.write(reinterpret_cast<char*>(&quark), sizeof(quark));
    }

    // close output file
    output.close();
}

StateNode::UP StateHistorySink::buildStateNode()
{
    // build node
    StateNode::UP node {new StateNode {_nextNodeId, this, _beginTs}};

    // update next node ID
    _nextNodeId++;

    return node;
}

std::size_t StateHistorySink::getNodesCount() const
{
    /* Here we want to know the number of existing nodes
     * only in the state tree, which is why we need to
     * visit all of them, and only count them if they
     * exist.
     */
    std::unique_ptr<StateNodeCounterVisitor> visitor {
        new StateNodeCounterVisitor
    };

    _root->accept(*visitor, 0xffffffff);

    return visitor->getCount();
}

void StateHistorySink::writeNodesMap()
{
    // YAJL generator context
    auto yajlGen = ::yajl_gen_alloc(nullptr);

    std::unique_ptr<TreeToJsonStateNodeVisitor> visitor {
        new TreeToJsonStateNodeVisitor {yajlGen, this}
    };

    _root->accept(*visitor, 0xffffffff);

    // write this JSON string to a file
    const unsigned char* buf;
    std::size_t len;

    ::yajl_gen_get_buf(yajlGen, &buf, &len);

    bfs::ofstream output;

    output.open(_nodesMapPath, std::ios::binary);

    if (output) {
        output.write(reinterpret_cast<const char*>(buf), len);

        // close output file
        output.close();
    }

    // free YAJL geenrator context
    ::yajl_gen_free(yajlGen);
}

}
}
