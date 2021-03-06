import * as Beam from 'beam';
import { AccountEntry } from '../account_directory_page';

/** Interface for a model representing a single group. */
export abstract class GroupModel {

  /** The group this model represents. */
  public abstract get group(): Beam.DirectoryEntry;

  /** Returns the accounts belonging to the group. */
  public abstract get accounts(): AccountEntry[];

  /** Loads this model. */
  public abstract load(): Promise<void>;
}
