#pragma once

#include <minecs/internals/entity.hpp>
#include <minecs/internals/traits.hpp>
#include <minecs/internals/sparse_set.hpp>

namespace minecs
{
    template <typename T, typename U, typename... Args>
    requires std::is_unsigned_v<U> && (sizeof...(Args) > 0) && is_ecs_v<ecs_descriptor<T, Args...>>
    class entity_view
    {
    public:
        using size_type = U;

        class iterator
        {
        public:
            iterator(T* ecs, sparse_set<entity<U>, size_type>* entities, std::size_t index, std::size_t end)
                : m_ecs(ecs), m_entities(entities), m_index(index), m_end(end)
            {
            }

            auto operator*() const
            {
                entity<U> target_entity = m_entities->get_dense()[m_index];

                return std::tuple<entity<U>, Args&...>(
                    target_entity,
                    m_ecs->template get_sparse_set<Args>()
                        .get_dense()[m_ecs->template get_sparse_set<Args>()
                                         .get_sparse()[target_entity.id]]...);
            }

            bool operator!=(const entity_view::iterator&) const
            {
                return m_index < m_end;
            }

            entity_view::iterator& operator++()
            {
                ++m_index;

                return *this;
            }

        private:
            T* m_ecs;

            sparse_set<entity<U>, size_type>* m_entities;

            std::size_t m_index;
            std::size_t m_end;
        };

        using const_iterator = const iterator;

        entity_view(T* ecs, sparse_set<entity<U>, size_type>& entities)
            : m_ecs(ecs), m_entities(&entities), m_end(entities.get_dense().size())
        {
        }

        [[nodiscard]] iterator begin()
        {
            return entity_view::iterator(m_ecs, m_entities, 0, m_end);
        }

        [[nodiscard]] iterator end()
        {
            return entity_view::iterator(m_ecs, m_entities, m_end, m_end);
        }

        [[nodiscard]] const_iterator begin() const
        {
            return entity_view::iterator(m_ecs, m_entities, 0, m_end);
        }

        [[nodiscard]] const_iterator end() const
        {
            return entity_view::iterator(m_ecs, m_entities, m_end, m_end);
        }

    private:
        T* m_ecs;

        std::size_t m_end;

        sparse_set<entity<U>, size_type>* m_entities;
    };
}
