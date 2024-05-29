#ifndef NETWORKING_MULTIPLEXER_H
#define NETWORKING_MULTIPLEXER_H

#include <vector>
#include <tuple>
using namespace std;


// template to get the i-th type from a variadic template
template<size_t INDEX, typename First, typename... Rest>
struct type_at_index : type_at_index<INDEX - 1, Rest...> {
};

// Specialization for the base case when the index is 0
template<typename First, typename... Rest>
struct type_at_index<0, First, Rest...> {
    using type = First;
};

// wrapper function
template<size_t INDEX, typename... TYPES>
using type_at_index_v = typename type_at_index<INDEX, TYPES...>::type;


template<typename DATA,
        typename... MULTIPLEXING_TYPES>
struct multiplexer {
private:
    static constexpr size_t no_multiplexing_types = sizeof...(MULTIPLEXING_TYPES);

    using array_id = size_t;
    array_id unique_id = 1;

    vector<DATA> stream_res; // result from the stream
    template<size_t CURR_INDEX = 0>
    void rec_stream_array(const tuple<MULTIPLEXING_TYPES...> &tuple) {
        if constexpr (CURR_INDEX < no_multiplexing_types) {

            for (size_t handler_in: get<CURR_INDEX>(maps)[get<CURR_INDEX>(tuple)]) {
                struct handler &curr_handler = handlers_pool[handler_in]; // the current handler
                if (curr_handler.last_id != unique_id) { // need to reset the handler's next requirement back to 0
                    curr_handler.last_id = unique_id;
                    curr_handler.reqs_satisfied = 0;
                }

                if (++curr_handler.reqs_satisfied == curr_handler.no_reqs) { // all requirements were met
                    stream_res.push_back(curr_handler.data);
                }
            }

            rec_stream_array<CURR_INDEX + 1>(tuple);
        }
    }

    struct handler {
        array_id last_id = 0;

        size_t no_reqs = 0; // number of requirements this handle has
        size_t reqs_satisfied = 0; // current number of requirements met

        DATA data; // the actual data related to this handler
        explicit handler(const DATA &data) : data(data) {}
    };

    vector<handler> handlers_pool;

    tuple< // for each type in the variadic templates
            map<MULTIPLEXING_TYPES, // store map of this type's possible values
                    vector<size_t> // and the indexes (into the handlers_pool) of handlers requiring to these values
            >...
    > maps;

public:
    void append_new_empty_handler(const DATA &data) {
        handlers_pool.emplace_back(data);
    }

    template<size_t TYPE_INDEX>
    void add_requirement_to_last_handler(type_at_index_v<TYPE_INDEX, MULTIPLEXING_TYPES...> val) {
        get<TYPE_INDEX>(maps)[val].push_back(handlers_pool.size() - 1);
        handlers_pool.back().no_reqs++;
    }

    vector<DATA> stream_array(const tuple<MULTIPLEXING_TYPES...> &arr) {
        unique_id++;

        stream_res.clear();
        rec_stream_array(arr);
        return stream_res;
    }
};



#endif //NETWORKING_MULTIPLEXER_H
