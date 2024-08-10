//
// Created by syeyoung on 8/10/24.
//

#include "kr_syeyoung_dungeonsguide_mod_dungeon_roomprocessor_waterpuzzle_Waterboard.h"
#include "annealing.h"
#include <iostream>

#define STR2(x) #x
#define STR(X) STR2(X)


void JavaHashMapToStlActionList(JNIEnv *env, jobject hashMap, Action* lists, int size, int* actualSize) {
    jclass mapClass = env->FindClass("java/util/Map");
    if (mapClass == NULL) {
        return;
    }
    jmethodID entrySet = env->GetMethodID(mapClass, "entrySet", "()Ljava/util/Set;");
    if (entrySet == NULL) {
        return;
    }
    jobject set = env->CallObjectMethod(hashMap, entrySet);
    if (set == NULL) {
        return;
    }
    // Obtain an iterator over the Set
    jclass setClass = env->FindClass("java/util/Set");
    if (setClass == NULL) {
        return;
    }
    jmethodID iterator =
            env->GetMethodID(setClass, "iterator", "()Ljava/util/Iterator;");
    if (iterator == NULL) {
        return;
    }
    jobject iter = env->CallObjectMethod(set, iterator);
    if (iter == NULL) {
        return;
    }
    // Get the Iterator method IDs
    jclass iteratorClass = env->FindClass("java/util/Iterator");
    if (iteratorClass == NULL) {
        return;
    }
    jmethodID hasNext = env->GetMethodID(iteratorClass, "hasNext", "()Z");
    if (hasNext == NULL) {
        return;
    }
    jmethodID next =
            env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
    if (next == NULL) {
        return;
    }
    // Get the Entry class method IDs
    jclass entryClass = env->FindClass("java/util/Map$Entry");
    if (entryClass == NULL) {
        return;
    }
    jmethodID getKey =
            env->GetMethodID(entryClass, "getKey", "()Ljava/lang/Object;");
    if (getKey == NULL) {
        return;
    }
    jmethodID getValue =
            env->GetMethodID(entryClass, "getValue", "()Ljava/lang/Object;");
    if (getValue == NULL) {
        return;
    }
    // Iterate over the entry Set
    int idx = 0;
    while (env->CallBooleanMethod(iter, hasNext)) {
        jobject entry = env->CallObjectMethod(iter, next);
        jstring key = (jstring) env->CallObjectMethod(entry, getKey);
        jobjectArray value = (jobjectArray) env->CallObjectMethod(entry, getValue);
        const char* keyStr = env->GetStringUTFChars(key, NULL);
        if (!keyStr) {  // Out of memory
            return;
        }

        std::vector<Point> pointList;

        int length = env->GetArrayLength(value);
        for (int i = 0; i < length; i++) {
            jobject point = env->GetObjectArrayElement(value, i);
            jint x = env->GetIntField(point, env->GetFieldID(env->GetObjectClass(point), "x", "I"));
            jint y = env->GetIntField(point, env->GetFieldID(env->GetObjectClass(point), "y", "I"));
            pointList.push_back(Point {static_cast<uint8_t>(x), static_cast<uint8_t>(y)});
        }


        lists[idx].name = std::string(keyStr);
        lists[idx].moves = 3;
        lists[idx].flips = pointList;

        env->DeleteLocalRef(entry);
        env->ReleaseStringUTFChars(key, keyStr);
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
        idx++;

        if (idx == size) break;
    }
    *actualSize = idx;
}


JNIEXPORT jobjectArray JNICALL Java_kr_syeyoung_dungeonsguide_mod_dungeon_roomprocessor_waterpuzzle_Waterboard_nativeSolve
    (JNIEnv *env, jobject obj, jdouble tempMult, jdouble targetTemp, jint targetIter, jint moves, jint cnt1, jint cnt2) {
    auto clazz = env->GetObjectClass(obj);

    auto board =  reinterpret_cast<jobjectArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "currentState", "[[Lkr/syeyoung/dungeonsguide/mod/dungeon/roomprocessor/waterpuzzle/fallback/Simulator$Node;")));
    auto target =  reinterpret_cast<jobjectArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "targets", "[Lkr/syeyoung/dungeonsguide/mod/dungeon/roomprocessor/waterpuzzle/fallback/Simulator$Pt;")));
    auto nonTarget =  reinterpret_cast<jobjectArray>(env->GetObjectField(obj, env->GetFieldID(clazz, "nonTargets", "[Lkr/syeyoung/dungeonsguide/mod/dungeon/roomprocessor/waterpuzzle/fallback/Simulator$Pt;")));
    auto flips = env->GetObjectField(obj, env->GetFieldID(clazz, "switchFlips", "Ljava/util/Map;"));

    Node nodes[HEIGHT][WIDTH] = {BLOCK, 0, false};

    int height= env ->GetArrayLength(board);
    if (height > HEIGHT) {
        env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "Board does not fit within " STR(WIDTH) " by " STR(HEIGHT));
        return nullptr;
    }

    for (int y = 0; y < height; y++) {
        auto boardRow = reinterpret_cast<jobjectArray>(env->GetObjectArrayElement(board, y));

        int width = env ->GetArrayLength(boardRow);
        if (width > WIDTH) {
            env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), "Board does not fit within " STR(WIDTH) " by " STR(HEIGHT));
            return nullptr;
        }

        for (int x = 0; x < width; x++) {
            jobject element = env->GetObjectArrayElement(boardRow, x);

            auto clazz = env->GetObjectClass(element);
            jint waterLevel = env->GetIntField(element, env->GetFieldID(clazz, "waterLevel", "I"));
            jobject type = env->GetObjectField(element, env->GetFieldID(clazz, "nodeType", "Lkr/syeyoung/dungeonsguide/mod/dungeon/roomprocessor/waterpuzzle/fallback/Simulator$NodeType;"));
            jboolean update = env->GetBooleanField(element, env->GetFieldID(clazz, "update", "Z"));

            jint typeOrdinal = env->CallIntMethod(type, env->GetMethodID(env->GetObjectClass(type), "ordinal", "()I"));


            nodes[y][x].waterLevel = waterLevel;
            nodes[y][x].update = update;
            if (typeOrdinal == 0) {
                nodes[y][x].nodeType = BLOCK;
            } else if (typeOrdinal == 1) {
                nodes[y][x].nodeType = AIR;
            } else if (typeOrdinal == 2) {
                nodes[y][x].nodeType = WATER;
            } else if (typeOrdinal == 3) {
                nodes[y][x].nodeType = SOURCE;
            }
        }
    }
    print(nodes);


    std::vector<Point> ptTargets;
    std::vector<Point> notTargets;
    {
        int targetLen = env ->GetArrayLength(target);
        for (int i = 0; i < targetLen; i++) {
            jobject point = env->GetObjectArrayElement(target, i);
            jint x = env->GetIntField(point, env->GetFieldID(env->GetObjectClass(point), "x", "I"));
            jint y = env->GetIntField(point, env->GetFieldID(env->GetObjectClass(point), "y", "I"));
            ptTargets.push_back(Point {static_cast<uint8_t>(x), static_cast<uint8_t>(y)});
        }
//        for (const auto &item: ptTargets) {
//            std::cout << unsigned(item.x) << "," << unsigned(item.y) << " ";
//        }
//        std::cout << std::endl;
    }
    {
        int targetLen = env ->GetArrayLength(nonTarget);
        for (int i = 0; i < targetLen; i++) {
            jobject point = env->GetObjectArrayElement(nonTarget, i);
            jint x = env->GetIntField(point, env->GetFieldID(env->GetObjectClass(point), "x", "I"));
            jint y = env->GetIntField(point, env->GetFieldID(env->GetObjectClass(point), "y", "I"));
            notTargets.push_back(Point {static_cast<uint8_t>(x), static_cast<uint8_t>(y)});
        }
//        for (const auto &item: notTargets) {
//            std::cout << unsigned(item.x) << "," << unsigned(item.y) << " ";
//        }
//        std::cout << std::endl;
    }


    Action nullAction = {"nothing", {}, 1};
    Action list[10];

    int size = 0;
    JavaHashMapToStlActionList(env, flips, list, 10, &size);

    std::vector<Action*> actions;
    actions.reserve(size);
    for (int i = 0; i < size; i++) {
        actions.push_back(list + i);
        list[i].moves = moves;
    }

//    for (const auto &item: actions) {
//        std::cout << item->name << std::endl;
//        for (const auto &item: item->flips) {
//            std::cout << unsigned(item.x) << ", " << unsigned(item.y) <<  " ";
//        }
//        std::cout << std::endl;
//    }


    std::vector<Action*> currentActions;
    for (int i = 0; i < cnt1; i++) {
        currentActions.push_back(&nullAction);
    }

    std::vector<int> idxes;
    size_t idx = currentActions.size();
    for (Action* availableAction : actions) {
        for (int j = 0; j < cnt2; j ++) {
            currentActions.push_back(availableAction); // add 15 actions.
            idxes.push_back(idx++);
        }
    }

//    for (const auto &item: currentActions) {
//        std::cout << item->name << std::endl;
//    }
//    for (const auto &item: idxes) {
//        std::cout << item << " ";
//    }
//    std::cout << std::endl;

    std::vector<Action*> solution = anneal(nodes, ptTargets, notTargets, currentActions, idxes, tempMult, targetTemp, targetIter);


    jclass  actionClazz = env->FindClass("kr/syeyoung/dungeonsguide/mod/dungeon/roomprocessor/waterpuzzle/Waterboard$Action");
    jobjectArray array = env->NewObjectArray(solution.size(), actionClazz, NULL);

    idx = 0;

    jmethodID  constructor = env->GetMethodID(actionClazz, "<init>", "(ILjava/lang/String;)V");
    for (Action* item: solution) {
        jstring  string = env->NewStringUTF(item->name.c_str());
        jobject  action = env->NewObject(actionClazz, constructor, item->moves, string);

        env->SetObjectArrayElement(array, idx, action);
        idx++;
    }


    return array;
}
