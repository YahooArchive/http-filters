/*
 * Copyright (c) 2015, Yahoo Inc. All rights reserved.
 * Copyrights licensed under the New BSD License.
 * See the accompanying LICENSE file for terms.
 */

#ifndef OPCODES_H
#define OPCODES_H

namespace http {
namespace filters {

struct Opcodes {
  enum OPCODES {
    /*
     * invalid instruction.
     * no arguments.
     */
    kNull,

    /*
     * skips the instruction.
     * no arguments.
     */
    kSkip,

    /*
     * Pushes the registers to the stack, jumps to address and executes.
     * 1st parameter: Mode, where:
     *  - kNone: executes independently of register result's value.
     *  - kAnd: executes while Result register value is true.
     *  - kOr: executes while Result register value is false.
     * 2nd parameter: Code's memory offset.
     * 3rd parameter: Counter of how many instructions to execute. Value 0 executes indefinitely.
     */
    kExecute,

    /*
     * Executes a single instruction from a given address.
     * Can be called recursively.
     * 1st parameter: Code's memory offset.
     */
    kExecuteSingle,

    /*
     * Pops the register stack. When stack is empty halts the execution.
     * no arguments.
     */
    kReturn,

    /*
     * Halts the execution.
     * no arguments.
     */
    kHalt,

    /*
     * None mode.
     * no arguments.
     */
    kNone,

    /*
     * And mode, implicitly executes a kReturn instruction on next false result.
     * no arguments.
     */
    kAnd,

    /*
     * Or mode, implicitly executes a kReturn instruction on next true result.
     * no arguments.
     */
    kOr,

    /*
     * Not flag, if true, flips the next valid result:
     *  - if true then false
     *  - else if false then true
     * no arguments.
     */
    kNot,

    /*
     * Flips the Result register to its opposite value:
     *  - if true then false
     *  - else if false then true
     * no arguments.
     */
    kFlip,

    /*
     * sets the Result register to false.
     * no arguments.
     */
    kFalse,

    /*
     * sets the Result register to true.
     * no arguments.
     */
    kTrue,

    /*
     * 1st parameter: Message's memory offset.
     * 2nd parameter: Tag's memory offset.
     * 3nd parameter: Mode, where:
     *  - kNone: prints the message independently of register result's value.
     *  - kAnd: prints the message only when register result's value is true.
     *  - kOr: prints the message only when register result's value is false.
     */
    kPrintError,
    kPrintDebug,

    /*
     * Method
     * 1st parameter: method.
     * 2nd parameter: strlen(1st argument).
     */
    kIsMethod,

    /*
     * Scheme
     * 1st parameter: scheme.
     * 2nd parameter: strlen(1st argument).
     */
    kIsScheme,

    /*
     * Domain
     * 1st parameter: Comparison value.
     */
    kContainsDomain,
    kEqualDomain,
    kNotEqualDomain,
    kStartsWithDomain,

    /*
     * Path
     * 1st parameter: Comparison value.
     */
    kContainsPath,
    kEqualPath,
    kNotEqualPath,
    kStartsWithPath,

    /*
     * Query Parameters
     * 1st parameter: Query Parameter name.
     * 2nd parameter: Comparison value.
     */
    kContainsQueryParameter,
    kEqualQueryParameter,
    kExistsQueryParameter,
    kGreaterThanQueryParameter,
    kGreaterThanAfterQueryParameter,
    kLessThanQueryParameter,
    kLessThanAfterQueryParameter,
    kNotEqualQueryParameter,
    kStartsWithQueryParameter,

    /*
     * Headers
     * 1st parameter: Header name.
     * 2nd parameter: Comparison value.
     */
    kContainsHeader,
    kEqualHeader,
    kExistsHeader,
    kGreaterThanHeader,
    kGreaterThanAfterHeader,
    kLessThanHeader,
    kLessThanAfterHeader,
    kNotEqualHeader,
    kStartsWithHeader,

    /*
     * Cookies
     * 1st parameter: Cookie name.
     * 2nd parameter: Comparison value.
     */
    kContainsCookie,
    kEqualCookie,
    kExistsCookie,
    kGreaterThanCookie,
    kGreaterThanAfterCookie,
    kLessThanCookie,
    kLessThanAfterCookie,
    kNotEqualCookie,

    /*
     * invalid instruction.
     * no arguments.
     */
    kUpperBound,
  };
};

struct ExecutionMode {
  enum MODES {
    kNull,

    kNone,
    kAnd,
    kOr,

    kUpperBound,
  };
};

} //end of filters namespace
} //end of http namespace

#endif
