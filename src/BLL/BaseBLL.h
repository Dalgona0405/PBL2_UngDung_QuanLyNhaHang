#ifndef BASE_BLL_H
#define BASE_BLL_H
#include "Result.h"
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

template <typename TEntity, typename TDAL>
class BaseBLL {
    protected:
        TDAL &dal;
        virtual Result validateEntity(const TEntity &entity, bool isUpdate = false) = 0;
        virtual string getEntityName() const = 0;

    public:
        BaseBLL(TDAL &dalRef) : dal(dalRef) {}
        virtual ~BaseBLL() {}
        virtual vector<TEntity> getAll() {
            try {
                return dal.getAll();
            } catch (const exception &e) {
                return vector<TEntity>();
            }
        }

        virtual bool getById(int id, TEntity &out) {
            if (id <= 0)
                return false;
            try {
                return dal.getById(id, out);
            } catch (const exception &e) {
                return false;
            }
        }

        virtual Result addChecked(const TEntity &entity) {
            Result r;
            try {
                r = validateEntity(entity, false);
                if (!r.ok)
                    return r;
                bool ok = dal.insert(entity);
                r.ok = ok;
                if (ok) {
                    r.message = "Them " + getEntityName() + " thanh cong";
                } else {
                    r.message = "Them " + getEntityName() + " that bai";
                }
            } catch (const exception &e) {
                r.ok = false;
                r.message = "Loi: " + string(e.what());
            }
            return r;
        }

        virtual Result updateChecked(const TEntity &entity) {
            Result r;
            try {
                TEntity exists;
                if (!dal.getById(entity.id, exists)) {
                    r.ok = false;
                    r.message = getEntityName() + " khong ton tai";
                    return r;
                }
                r = validateEntity(entity, true);
                if (!r.ok)
                    return r;
                bool ok = dal.update(entity);
                r.ok = ok;
                if (ok) {
                    r.message = "Sua " + getEntityName() + " thanh cong";
                } else {
                    r.message = "Sua " + getEntityName() + " that bai";
                }
            } catch (const exception &e) {
                r.ok = false;
                r.message = "Loi: " + string(e.what());
            }
            return r;
        }

        virtual Result removeChecked(int id) {
            Result r;
            try {
                TEntity exists;
                if (!dal.getById(id, exists)) {
                    r.ok = false;
                    r.message = getEntityName() + " khong ton tai";
                    return r;
                }
                bool ok = dal.remove(id);
                r.ok = ok;
                if (ok) {
                    r.message = "Xoa " + getEntityName() + " thanh cong";
                } else {
                    r.message = "Xoa " + getEntityName() + " that bai";
                }
            } catch (const exception &e) {
                r.ok = false;
                r.message = "Loi: " + string(e.what());
            }
            return r;
        }
};
#endif