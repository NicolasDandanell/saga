#ifndef SAGA_REQUIREMENTS_H
#define SAGA_REQUIREMENTS_H

/**
 * @requirement saga_requirement_00 Saga C11 requirement
 *
 * Saga Requires a C11 compliant compiler to function
 */

/**
 * @requirement saga_requirement_01 Saga log size requirement
 *
 * A Saga log entry must not be larger than 255 bytes.
 * This is due to the first byte of the entry being the size, and thus being limited to a max value of 255.
 */

/**
 * @requirement saga_requirement_02 Saga message format requirement
 *
 * A Saga log message must be formatted using the printf standard.
 * You can read more about the printf format here: https://en.wikipedia.org/wiki/Printf
 */

#endif // SAGA_REQUIREMENTS_H
